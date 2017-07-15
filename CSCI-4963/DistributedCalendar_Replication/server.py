import sys
import socket
import threading
import json
import os.path
import time
from flask import Flask
from flask import render_template
from flask import request
from flask import redirect
from parse import *


# ------ Global variables ------
TotalNodes = 4                             # Total number of nodes
"""
NodeData = {                               # Maps node ID to (Name, IP, WebPort, TcpPort)
    1: ("Alan", "52.88.92.72", 80, 40061),                        
    2: ("Bob", "52.18.229.252", 81, 40062),
    3: ("Carl", "52.69.215.52", 82, 40063),
    4: ("David", "54.207.62.34", 83, 40064)
}
"""
NodeData = {                               # Maps node ID to (Name, IP, WebPort, TcpPort)
    1: ("Alan", "127.0.0.1", 80, 40061),                        
    2: ("Bob", "127.0.0.1", 81, 40062),
    3: ("Carl", "127.0.0.1", 82, 40063),
    4: ("David", "127.0.0.1", 83, 40064)
}
NodeId = None                              # Node ID (1 - 4)
WebApp = Flask("WebServer")                # Flask app instance
IndexPage = "index.html"                   # Path to index HTML page
LogFile = "log_%d.txt"                     # Path to log file
CalendarFile = "calendar_%d.txt"           # Path to calendar file
WebThread = None                           # Thread for the Flask web server
ListenThread = None                        # Thread for the TCP socket server

Clock = 0                                                               # Clock
Matrix = [[0 for x in range(TotalNodes)] for x in range(TotalNodes)]    # NxN matrix
MyLog = None                                                            # Log
MyCalendar = None                                                       # Calendar   
MyAlerts = []                                                           # UI Alerts

OP_INSERT = "INSERT"
OP_DELETE = "DELETE"
# ------------------------------

# Appointment class
class Appointment:    
    def __init__(self, name, day, startTime, endTime, participants):
        self.name = name
        self.day = day
        self.startTime = startTime
        self.endTime = endTime
        self.participants = participants
        

# Event class
class Event:
    def __init__(self, opType, appointment, time, nodeId):
        self.opType = opType
        self.appointment = appointment
        self.time = time
        self.nodeId = nodeId
        
    def hasrec(self, k):
        return Matrix[k - 1][self.nodeId - 1] >= self.time
    
    
# Log class
class Log:
    def __init__(self):
        self.events = []
        self.load()
        
    def addEvent(self, opType, apptName):
        global Clock
        global Matrix
        global NodeId
        
        Clock += 1
        Matrix[NodeId - 1][NodeId - 1] = Clock
        event = Event(opType, apptName, Clock, NodeId)
        self.events.append(event)
        self.save()
        
    def addInsertEvent(self, appointment):
        self.addEvent(OP_INSERT, appointment)
        
    def addDeleteEvent(self, appointment):
        self.addEvent(OP_DELETE, Appointment(appointment.name, None, None, None, None))
        
    def containsEvent(self, event):
        return len([e for e in self.events if e.opType == event.opType and e.appointment.name == event.appointment.name]) > 0
        
    def load(self):
        global Clock
        global Matrix
        
        if not os.path.isfile(LogFile):
            return
            
        file = open(LogFile, "r")
        lines = file.readlines()
        file.close()
        readAppointmentData = False
        
        for line in lines:
            if (len(line) == 0):
                continue
                
            # After reading an INSERT log event, we need to read the JSON-encoded appointment data next
            # So if we encounter an INSERT event, then set the boolean flag. For DELETE events, this flag
            # does not need to be touched.
            
            if not readAppointmentData:
                result = parse("{} '{}', node {}, clock {}", line)
                opType = result[0]
                apptName = result[1]
                nodeId = int(result[2])
                clockTime = int(result[3])
                
                if result[0] == OP_INSERT:
                    self.events.append(Event(opType, None, clockTime, nodeId))
                    readAppointmentData = True
                    
                    # Recover the clock value based on the highest clockTime of an event created by this node
                    if clockTime > Clock and NodeId == nodeId:
                        Clock = clockTime
                        Matrix[NodeId - 1][NodeId - 1] = Clock
                    
                elif result[0] == OP_DELETE:
                    self.events.append(Event(opType, Appointment(apptName, None, None, None, None), clockTime, nodeId))
                    readAppointmentData = False
                    
            else:
                d = json.loads(line)
                appointment = Appointment(d["name"], d["day"], d["startTime"], d["endTime"], d["participants"])
                self.events[len(self.events) - 1].appointment = appointment
                readAppointmentData = False
        
    def save(self):
        file = open(LogFile, "w")
        for e in self.events:
            file.write("%s '%s', node %d, clock %d\n" % (e.opType, e.appointment.name, e.nodeId, e.time))
            
            if e.opType == OP_INSERT:
                file.write(json.dumps(e.appointment.__dict__))
                file.write("\n")
        file.close()
    
    
# Calendar class
class Calendar:
    def __init__(self):
        self.appointments = []
        self.load()
        
    def add(self, appointment):
        if self.contains(appointment):
            return False
        self.appointments.append(appointment)
        self.save()
        return True
        
    def remove(self, appointment):
        originalSize = len(self.appointments)
        self.appointments = [app for app in self.appointments if app.name != appointment.name]
        self.save()
        return len(self.appointments) != originalSize
        
    def conflict(self, appointment):
        timeSlots = [[[0 for x in range(48)] for x in range(7)] for x in range(4)]
        for app in self.appointments:
            for x in range(app.startTime/30, app.endTime/30):
                for p in app.participants:
                    timeSlots[p-1][app.day-1][x] = 1;
        
        for x in range (appointment.startTime/30, appointment.endTime/30):
            for p in appointment.participants:
                if (timeSlots[p-1][appointment.day-1][x] == 1) :
                    return True
        return False
        
    def contains(self, appointment):
        return len([app for app in self.appointments if app.name == appointment.name]) > 0
        
    def load(self):
        if not os.path.isfile(CalendarFile):
            return
            
        file = open(CalendarFile, "r")
        data = json.loads(file.read())
        file.close()
        
        for d in data:
            appointment = Appointment(d["name"], d["day"], d["startTime"], d["endTime"], d["participants"])
            self.appointments.append(appointment)
        
    def save(self):
        data = json.dumps([app.__dict__ for app in self.appointments])
        file = open(CalendarFile, "w")
        file.write(data)
        file.close()

        
# Message class
class Message:
    def __init__(self, events = None, matrix = None, sourceId = None):
        self.events = events
        self.matrix = matrix
        self.sourceId = sourceId
        
    """
    Serialization format:
    --------------------------------
    NodeId
    number of events (n)
    opType                  }
    time                    }  repeated n times
    nodeId                  }  
    appointmentJSON         }
    matrixRowCount
    matrix line 1           }
    matrix line 2           }  in csv form (eg: 1,2,3,4) for each line
    matrix line 3           }
    matrix line 4           }
    """
    
    def serialize(self):
        lines = []
        lines.append(str(NodeId))
        lines.append(str(len(self.events)))
        
        for e in self.events:
            lines.append(e.opType)
            lines.append(str(e.time))
            lines.append(str(e.nodeId))
            lines.append(json.dumps(e.appointment.__dict__))
            
        lines.append(str(len(self.matrix)))
        
        for x in range(len(self.matrix)):
            lines.append(",".join([str(c) for c in self.matrix[x]]))
            
        return "\n".join(lines)
        
        
    def unserialize(self, input):
        
        # Reset internal state
        self.events = []
        self.matrix = []

        # Basic error checking
        if not input or len(input) == 0:
            return False
        
        lines = input.split("\n")
        if len(lines) == 0:
            return False
        
        self.sourceId = int(lines[0])
        eventCount = int(lines[1])
        currentLine = 2
        
        # Read events data
        for x in range(eventCount):
            if currentLine + 3 >= len(lines):
                return False
                
            opType = lines[currentLine]
            time = int(lines[currentLine + 1])
            nodeId = int(lines[currentLine + 2])
            d = json.loads(lines[currentLine + 3])
            appointment = Appointment(d["name"], d["day"], d["startTime"], d["endTime"], d["participants"])
            event = Event(opType, appointment, time, nodeId)
            
            self.events.append(event)
            currentLine += 4
            
            
        # Read matrix data
        matrixRowCount = int(lines[currentLine])
        currentLine += 1
        
        if currentLine + matrixRowCount - 1 > len(lines):
            return False
            
        for x in range(matrixRowCount):
            self.matrix.append([int(x) for x in lines[currentLine].split(",")])
            currentLine += 1
            
        return True
        
        
# Notify nodes that they are a participant in an appointment
def notifyParticipants(appointment):
    for k in [x for x in appointment.participants if x != NodeId]:
        NP = [e for e in MyLog.events if not e.hasrec(k)]
        m = Message(NP, Matrix)
        
        print "[DSP1] Sending %d events in total to %s" % (len(NP), NodeData[k][0])
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((NodeData[k][1], NodeData[k][3]))
            s.sendall(m.serialize())
            s.close()
        except:
            pass
        

# Web application user interface
@WebApp.route("/")
def web_root():
    global MyAlerts
    
    appointmentsJSON = json.dumps([app.__dict__ for app in MyCalendar.appointments])
    logFileContents = open(LogFile).read() if os.path.isfile(LogFile) else "(Log is empty)"
    alertJSON = json.dumps(MyAlerts)
    
    MyAlerts = []
    return render_template(IndexPage, id = NodeId, name = NodeData[NodeId][0], calendar = appointmentsJSON, log = logFileContents, alerts = alertJSON)
    
    
# Creating an appointment
@WebApp.route("/create", methods=["POST"])
def web_create():
    name = request.form.get("name", None)
    day = request.form.get("day", None)
    start_time = request.form.get("start_time", None)
    end_time = request.form.get("end_time", None)
    participants = []
    
    if request.form.get("alan", None) != None:   participants.append(1)
    if request.form.get("bob", None) != None:    participants.append(2)
    if request.form.get("carl", None) != None:   participants.append(3)
    if request.form.get("david", None) != None:  participants.append(4)
       
       
    # Process data 
    appointment = Appointment(name, int(day), int(start_time), int(end_time), participants) 
          
    if not MyCalendar.conflict(appointment):
        MyLog.addInsertEvent(appointment)
        MyCalendar.add(appointment)
        notifyParticipants(appointment)
    else:
        MyAlerts.append("Cannot insert '%s' into calendar because of a conflict" % (name))
        
    return redirect("/", code = 302)
    

# Deleting an appointment
@WebApp.route("/delete", methods=["POST"])
def web_delete():
    appointmentName = request.form.get("id", "").lower().replace("___", " ")
    appointmentQuery = [app for app in MyCalendar.appointments if app.name.lower() == appointmentName]
    
    if len(appointmentQuery) > 0:
        appointment = appointmentQuery[0]
        MyLog.addDeleteEvent(appointment)
        MyCalendar.remove(appointment)
        notifyParticipants(appointment)
        
    return redirect("/", code = 302)

    
# Page not found
@WebApp.errorhandler(404)
def web_notfound(error):
    return "Page not found"


# Flask web server thread
def webServer(nodeId):
    WebApp.run(host = "0.0.0.0", port = NodeData[NodeId][2], debug = True, use_reloader = False)
    
    
# TCP socket server thread
def tcpServer():
    listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listener.bind(("0.0.0.0", NodeData[NodeId][3]))
    listener.listen(TotalNodes)
    
    bufferSize = 8192       # Number of bytes to store in buffer for every recv() call
    buffers = {}            # Maps IP address to buffer contents
    
    while True:
        (connection, address) = listener.accept()
        data = connection.recv(bufferSize)
        m = Message()
        
        if not address in buffers:
            buffers[address] = ""
        buffers[address] += data
        
        print "[DSP1] Received from %s: %s" % (address, data)
        if not m.unserialize(data):
            print "[DSP1] Invalid message format, discarding"
            continue
        
        
        # Compute NE (new events)
        unsortedNE = [f for f in m.events if not f.hasrec(NodeId)]
        tempNE = [e for e in unsortedNE if e.opType == OP_DELETE] + [e for e in unsortedNE if e.opType == OP_INSERT]
        conflicts = []
        
        
        # Remove insert/delete pairs of the same appointment in NE
        NE = []
        appts = {}
        
        for e in tempNE:
            if e.opType == OP_DELETE:
                appts[e.appointment.name] = 1
            elif e.opType == OP_INSERT:
                if e.appointment.name in appts and appts[e.appointment.name] == 1:
                    appts[e.appointment.name] = 2
                else:
                    appts[e.appointment.name] = 1
                    
        NE = [e for e in tempNE if appts[e.appointment.name] == 1]
        
        
        # Process new events
        for e in NE:
            if e.opType == OP_INSERT and not MyCalendar.contains(e.appointment):
                print "[DSP1] Adding event %s" % (e.appointment.name)
                if MyCalendar.conflict(e.appointment):
                    conflicts.append(e.appointment)
                MyCalendar.add(e.appointment)
                
            elif e.opType == OP_DELETE and MyCalendar.contains(e.appointment):
                print "[DSP1] Removing event %s" % (e.appointment.name)
                MyCalendar.remove(e.appointment)
                
                
        # Matrix update
        for i in range(TotalNodes):
            Matrix[NodeId - 1][i] = max(Matrix[NodeId - 1][i], m.matrix[m.sourceId - 1][i])
            
        for i in range(TotalNodes):
            for j in range(TotalNodes):
                Matrix[i][j] = max(Matrix[i][j], m.matrix[i][j])
        
        
        # Update the local log based on NE
        union = MyLog.events + NE
        tempLog = Log()
        
        for e in union:
            necessary = False
            for j in range(TotalNodes):
                if not e.hasrec(j + 1):
                    necessary = True
                    break
                    
            if necessary and not tempLog.containsEvent(e):
                tempLog.events.append(e)
                
        MyLog.events = tempLog.events
        MyLog.save()
        
        
        # Conflict resolution protocol
        # Send DELETE events to all participants for conflicting appointments
        for a in conflicts:
            print "[DSP1] Deleting conflict appointment %s" % (a.name)
            MyLog.addDeleteEvent(a)
            MyCalendar.remove(a)
            notifyParticipants(a)

        MyCalendar.save()
        MyLog.save()
        
        
# Program execution officially starts here
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "Usage: python server.py [nodeId]"
        sys.exit(1)
          
    NodeId = int(sys.argv[1])
    LogFile = LogFile % (NodeId)
    CalendarFile = CalendarFile % (NodeId)
    
    if NodeId < 1 or NodeId > TotalNodes:
        print "Invalid node ID, cannot start server!"
        sys.exit(1)
        
    print "[DSP1] Starting application with nodeId = %d" % (NodeId)
    print "[DSP1] Web server running on port %d, TCP socket server running on port %d" % (NodeData[NodeId][2], NodeData[NodeId][3])
    
    MyLog = Log()
    MyCalendar = Calendar()
    
    WebThread  = threading.Thread(target = webServer, args = (NodeId,))
    WebThread.daemon = True     # So that the process will also be killed with main
    WebThread.start()
    
    ListenThread = threading.Thread(target = tcpServer)
    ListenThread.daemon = True
    ListenThread.start()
    
    
    # Infinite loop
    while threading.active_count() > 0:
        time.sleep(0.1)