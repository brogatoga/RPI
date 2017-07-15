import sys
import socket
import threading
import json
import os.path
import time
import logging
import math
from flask import Flask, render_template, request, redirect
from parse import *


# ------ Global variables ------
# Maps node ID to (Name, Location, AWS_IP, (LocalWebPort, LocalTcpPort, LocalUdpPort), (AWSWebPort, AWSTcpPort, AWSUdpPort))
NodeData = {                               
    1: ("Alan", "Oregon", "52.88.92.72", (80, 40061, 40066), (80, 40061, 40070)),                        
    2: ("Bob", "Ireland", "52.18.229.252", (81, 40062, 40067), (80, 40061, 40070)),
    3: ("Carl", "Tokyo", "52.69.215.52", (82, 40063, 40068), (80, 40061, 40070)),
    4: ("David", "Sao Paulo", "54.207.62.34", (83, 40064, 40069), (80, 40061, 40070)),
    5: ("Earl", "Singapore", "54.169.235.208", (84, 40065, 40070), (80, 40061, 40070))
}

TotalNodes = 5                             # Total number of nodes
Majority = math.ceil(TotalNodes / 2)       # Majority required
NodeId = None                              # Node ID (1 - 5)
WebApp = Flask("WebServer")                # Flask app instance
IndexPage = "index.html"                   # Path to index HTML page
LogFile = "log_%d.txt"                     # Path to log file
StateFile = "pn_%d.txt"                    # Path to Paxos stable storage file
LogFileLock = False                        # Is the log file currently being written to?
StateFileLock = False                      # Is the state file currently being written to?
WebThread = None                           # Thread for the Flask web server
ListenThread = None                        # Thread for the TCP socket server
Localhost = False                          # Are we running on localhost?
Reload = False                             # Do we need to reload?

# Ports that we're using
WebPort = None                             
TcpPort = None                             
UdpPort = None

MyLog = None                                                            # Log
MyCalendar = None                                                       # Calendar
MyState = None                                                          # Paxos acceptor state   
MyAlerts = []                                                           # UI Alerts
LeaderId = None                                                         # Leader node ID
LeaderState = None                                                      # Leader election state
LeaderElectionStartTime = None                                          # Last time we initiated an election
LeaderLastHeartbeat = None                                              # Last time we sent a heartbeat to the leader
PatchQueue = []                                                         # Queue of already-committed events that we need information on (first priority)
ReadyQueue = []                                                         # Queue of new events that we want to add to the log (second priority)
ProposalNumberState = None                                              # Proposal number state (do we know the highest proposal number?)
ProposalNumberResponses = []                                            # List of responses from asking all other nodes for the highest proposal number

PaxosState = None                                                       # Paxos proposer state (for leader/DP only)
PaxosIsPatching = False                                                 # Are we currently using Paxos to patch our local log?
PaxosPendingEvent = None                                                # Pending event for Paxos algorithm
PaxosEventIndex = None                                                  # Index of the event (position in log) we're currently processing
PaxosPromiseResponses = None                                            # List of promise responses received from a proposal request
PaxosACKNum = None                                                      # The number of ACK responses received from an accept request
PaxosTimer = None                                                       # Timer for Paxos timeout detection
PaxosForwardingTracker = {}                                             # Tracks forwarded events until an acknowledgement is received (maps eventDataString to time sent)
PaxosForwardingQueue = []                                               # Queue of forwarded events that were never successfully sent to the leader

# Protocol strings, states, and configurations
TCP = 1
UDP = 2
LOCALHOST_IP = "127.0.0.1"
BUFFER_SIZE = 8192

MSG_RELOAD_BASE = "RELOAD"
MSG_RELOAD = MSG_RELOAD_BASE + "-%d"                                    # Format: RELOAD-senderID
MSG_BROADCAST_PN_BASE = "BROADCAST_PROPOSAL_NUMBER"
MSG_BROADCAST_PN = MSG_BROADCAST_PN_BASE + "-%d-%d"                     # Format: BROADCAST_PROPOSAL_NUMBER-senderID-newProposalNumber
MSG_REQUEST_PN_BASE = "REQUEST_PROPOSAL_NUMBER"
MSG_REQUEST_PN = MSG_REQUEST_PN_BASE + "-%d"                            # Format: REQUEST_PROPOSAL_NUMBER-senderID

OP_INSERT = "INSERT"
OP_DELETE = "DELETE"

BLE_OK = "OK-%d"
BLE_ELECTION = "ELECTION-%d"
BLE_COORDINATOR = "COORDINATOR-%d"
BLE_HEARTBEAT = "HELLO"

PRN_STATE_STABLE = 1                    # We know the highest proposal number
PRN_STATE_INPROGRESS = 2                # We're currently requesting the highest proposal number
PRN_STATE_UNKNOWN = 3                   # We don't know the highest proposal number yet

BLE_STATE_STABLE = 1                    # We know the leader node's ID
BLE_STATE_INPROGRESS = 2                # We're currently holding an election
BLE_STATE_UNKNOWN = 3                   # We don't know the leader node's ID yet

BLE_HEARTBEAT_FREQUENCY = 3             # Send heartbeat message every X seconds
BLE_RESPONSE_TIMEOUT = 5                # Number of seconds to wait for a response after sending an ELECTION message

PXS_FORWARD = "PXS_FORWARD"             # Format: PXS_FORWARD|eventData|senderID         (Proposer forwarding event data to the distinguished proposer (leader))
PXS_FORWARD_ACK = "PXS_FWD_ACK"         # Format: PXS_FWD_ACK|eventData|senderID         (Leader acknowledges that the specific event was received)
PXS_FORWARD_CONFLICT = "PXS_FWD_CONF"   # FORMAT: PXS_FWD_CONF|eventData|senderID        (Leader notifies sender that this event caused a conflict)
PXS_PREPARE = "PXS_PREPARE"             # Format: PXS_PREPARE|proposalNumber|eventData|senderID
PXS_PROMISE = "PXS_PROMISE"             # Format: PXS_PROMISE|accNum|eventData|senderID
PXS_ACCEPT = "PXS_ACCEPT"               # Format: PXS_ACCEPT|proposalNumber|eventData|senderID
PXS_ACK = "PXS_ACK"                     # Format: PXS_ACK|accNum|eventData|senderID
PXS_COMMIT = "PXS_COMMIT"               # Format: PXS_COMMIT|eventData|senderID
PXS_PROBE = "PXS_PROBE"                 # Format: PXS_PROBE|eventIndex|senderID          (Node asks another node if it has the event at eventIndex)

PXS_TIMEOUT = 5                         # Number of seconds Paxos will wait for "promise" and "ack" rseponses
PXS_PATCH_TIMEOUT = 3                   # Number of seconds Paxos will wait for a patch response

PXS_STATE_PATCHING = 0                  # Our log is outdated and we are currently running Synod algorithm to update it
PXS_STATE_READY = 1                     # Our log is up to date and we are ready to process new proposals
PXS_STATE_PROPOSING = 2                 # Someone just proposed a new event. Send to all the nodes and wait for response (promise or nothing)
PXS_STATE_ACCEPTING = 3                 # Phase 1 finished. Send accept to all the nodes and wait for ack
PXS_STATE_COMMITTED = 4                 # Received enough acks and sent commit to all the nodes

# ------------------------------


# Appointment class
class Appointment:    
    def __init__(self, name, day, startTime, endTime, participants):
        self.name = name
        self.day = day
        self.startTime = startTime
        self.endTime = endTime
        self.participants = participants
        
    def serialize(self):
        return json.dumps(self.__dict__)
        
    def unserialize(self, data):
        d = json.loads(data)
        self.name = d["name"]
        self.day = d["day"]
        self.startTime = d["startTime"]
        self.endTime = d["endTime"]
        self.participants = d["participants"]


# Event class
class Event:
    def __init__(self, eventNum, opType, appointment, nodeId):
        self.eventNum = eventNum
        self.opType = opType
        self.appointment = appointment
        self.nodeId = nodeId
        
    def serialize(self):
        d = {
            "eventNum": self.eventNum,
            "opType": self.opType,
            "nodeId": self.nodeId,
            "appointment": self.appointment.serialize() if self.appointment != None else ""
        }
        return json.dumps(d)
        
    def unserialize(self, data):
        d = json.loads(data)
        self.eventNum = d["eventNum"]
        self.opType = d["opType"]
        self.nodeId = d["nodeId"]
        self.appointment = Appointment(None, None, None, None, None)
        
        if len(d["appointment"]) > 0:
            self.appointment.unserialize(d["appointment"])
        
    
# Log class
class Log:
    def __init__(self):
        self.events = []
        self.load()
        
    def addEvent(self, event):
        self.events.append(event)
        self.events.sort(key = lambda x: x.eventNum)
        self.save()
        
    def containsEvent(self, event):
        return len([e for e in self.events if e.opType == event.opType and e.appointment.name == event.appointment.name]) > 0
        
    def containsEventIndex(self, eventNum):
        return len([e for e in self.events if e.eventNum == eventNum]) > 0
        
    def load(self):
        global LogFileLock
        
        if not os.path.isfile(LogFile):
            return
            
        while LogFileLock:
            time.sleep(0.05)
            
        LogFileLock = True
        file = open(LogFile, "r")
        lines = file.readlines()
        file.close()
        LogFileLock = False
        
        readAppointmentData = False
        
        for line in lines:
            if (len(line) == 0):
                continue
                
            # After reading an INSERT log event, we need to read the JSON-encoded appointment data next
            # So if we encounter an INSERT event, then set the boolean flag. For DELETE events, this flag
            # does not need to be touched.
            
            if not readAppointmentData:
                result = parse("Event# {}: {} '{}', node {}", line)
                eventNum = int(result[0])
                opType = result[1]
                apptName = result[2]
                nodeId = int(result[3])
                
                if result[1] == OP_INSERT:
                    self.events.append(Event(eventNum, opType, None, nodeId))
                    readAppointmentData = True
                    
                elif result[1] == OP_DELETE:
                    self.events.append(Event(eventNum,opType, Appointment(apptName, None, None, None, None), nodeId))
                    readAppointmentData = False
                    
            else:
                d = json.loads(line)
                appointment = Appointment(d["name"], d["day"], d["startTime"], d["endTime"], d["participants"])
                self.events[len(self.events) - 1].appointment = appointment
                readAppointmentData = False
        
    def save(self):
        global LogFileLock
        
        while LogFileLock:
            time.sleep(0.05)
            
        LogFileLock = True
        file = open(LogFile, "w")
        for e in self.events:
            file.write("Event# %d: %s '%s', node %d\n" % (e.eventNum, e.opType, e.appointment.name, e.nodeId))
            
            if e.opType == OP_INSERT:
                file.write(json.dumps(e.appointment.__dict__))
                file.write("\n")
        file.close()
        LogFileLock = False
    
    
# Calendar class
class Calendar:
    def __init__(self):
        self.appointments = []
        self.load(MyLog)
        
    def add(self, appointment):
        if self.contains(appointment):
            return False
        self.appointments.append(appointment)
        return True
        
    def remove(self, appointment):
        originalSize = len(self.appointments)
        self.appointments = [app for app in self.appointments if app.name != appointment.name]
        return len(self.appointments) != originalSize
        
    def clear(self):
        self.appointments = []
        
    def conflict(self, appointment):
        timeSlots = [[[0 for x in range(48)] for x in range(7)] for x in range(TotalNodes)]
        for app in self.appointments:
            for x in range(app.startTime / 30, app.endTime / 30):
                for p in app.participants:
                    timeSlots[p - 1][app.day - 1][x] = 1;
        
        for x in range (appointment.startTime / 30, appointment.endTime / 30):
            for p in appointment.participants:
                if (timeSlots[p - 1][appointment.day - 1][x] == 1) :
                    return True
        return False
        
    def contains(self, appointment):
        return len([app for app in self.appointments if app.name == appointment.name]) > 0
            
    # reload the appointments from reload events
    def load(self, log):
        self.clear()                            # Clear the appointments list in case load() is called a second time externally
        for e in log.events:
            if e.opType == OP_INSERT:
                self.add(e.appointment)
            else:
                self.remove(e.appointment)
                
                
# Paxos Stable Storage class
class PaxosStorage:
    def __init__(self):
        self._changed = False       # Has a change been made since the last save?
        self.highestProposal = 0    # Highest generated proposal number
        self.data = {}              # Maps eventNum to [maxPrepare, accNum, accVal], where:
        self.load()
        
    def _initKey(self, eventNum):
        if eventNum not in self.data:
            self.data[eventNum] = [-1, -1, "NULL"]      # Safe default values for each field that won't cause issues when writing to stable storage
        
    def setHighestProposal(self, value):
        self.highestProposal = value
        self._changed = True
        
    def setMaxPrepare(self, eventNum, value):
        self._initKey(eventNum)
        self.data[eventNum][0] = value
        self._changed = True
        
    def setAccNum(self, eventNum, value):
        self._initKey(eventNum)
        self.data[eventNum][1] = value
        self._changed = True
        
    def setAccVal(self, eventNum, value):
        self._initKey(eventNum)
        self.data[eventNum][2] = value
        self._changed = True
        
    def getHighestProposal(self):
        return self.highestProposal
        
    def getMaxPrepare(self, eventNum):
        return self.data[eventNum][0] if eventNum in self.data and self.data[eventNum][0] != -1 else None
            
    def getAccNum(self, eventNum):
        return self.data[eventNum][1] if eventNum in self.data and self.data[eventNum][1] != -1 else None
        
    def getAccVal(self, eventNum):
        return self.data[eventNum][2] if eventNum in self.data and self.data[eventNum][2] != "NULL" else None
    
    def save(self):
        global StateFileLock
        
        """
        Format:
        highestProposal
        dictionarySize (n)
        eventNum                 }
        maxPrepare               }  repeated n times
        accNum                   }  
        accVal                   }
        """
        
        # If nothing has changed, then we don't need to save
        if not self._changed:
            return
    
        while (StateFileLock):
            time.sleep(0.05)
            
        StateFileLock = True
        file = open(StateFile, "w")
        file.write("%d\n%d" % (self.highestProposal, len(self.data)))
    
        for eventNum, data in self.data.iteritems():
            file.write("\n%d\n%d\n%d\n%s" % (eventNum, self.data[eventNum][0], self.data[eventNum][1], self.data[eventNum][2]))
        
        self._changed = False
        file.close()
        StateFileLock = False

    def load(self):
        global StateFileLock
        
        if not os.path.isfile(StateFile):
            return

        while (StateFileLock):
            time.sleep(0.05)
            
        StateFileLock = True
        file = open(StateFile, "r")
        lines = file.readlines()
        file.close()
        StateFileLock = False
    
        if lines == None or len(lines) == 0:
            return
            
        self.highestProposal = int(lines[0])
        dictSize = int(lines[1])
        currentLine = 2
    
        if dictSize == 0:
            return
        
        for x in range(dictSize):
            while len(lines[currentLine]) == 0:
                currentLine += 1
                
            if currentLine + 3 > len(lines):
                break
          
            eventNum = int(lines[currentLine]) if lines[currentLine].isdigit() else None
            maxPrepare = int(lines[currentLine + 1]) if lines[currentLine + 1].isdigit() else -1
            accNum = int(lines[currentLine + 2]) if lines[currentLine + 2].isdigit() else -1
            accVal = lines[currentLine + 3] if len(lines[currentLine + 3]) > 0 else "NULL"
        
            if eventNum != None and eventNum >= 0:
                self.data[eventNum] = [maxPrepare, accNum, accVal]
                
            currentLine += 4
    
    
# Send the specified message to the node with given ID, returns success status
def sendToNode(message, id, method, suppressLogging = False):
    if id < 1 or id > TotalNodes or id == None or (method != TCP and method != UDP):
        return
        
    index = 3 if Localhost else 4
    ip = LOCALHOST_IP if Localhost else NodeData[id][2]
    port = NodeData[id][index][method]
    
    if not suppressLogging:
        print "[DSP2] Sending '%s' to node %d (%s) at %s:%d" % (message, id, "TCP" if method == 1 else "UDP", ip, port)
    
    try:
        if method == TCP:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((ip, port))
            s.sendall(message)
            s.close()
        elif method == UDP:
            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            s.sendto(message, (ip, port))
    except:
        return False
    return True
    
    
# Resets the Paxos state variables
def resetPaxosState():
    global PaxosTimer, PaxosPendingEvent, PaxosEventIndex, PaxosPromiseResponses, PaxosACKNum, PaxosState, PaxosIsPatching
    
    PaxosTimer = None
    PaxosPendingEvent = None
    PaxosEventIndex = None
    PaxosPromiseResponses = []
    PaxosACKNum = 0
    PaxosState = PXS_STATE_PATCHING if PaxosIsPatching else PXS_STATE_READY
    
    
# Starts a new election
def startElection():
    global LeaderElectionStartTime, LeaderState
    
    if NodeId < TotalNodes:
        LeaderState = BLE_STATE_INPROGRESS
        LeaderElectionStartTime = time.time()
        
        for id in range(NodeId + 1, TotalNodes + 1):
            sendToNode(BLE_ELECTION % (NodeId), id, TCP)
            
    elif NodeId == TotalNodes:
        announceLeader()
            

# Announce this node as the new leader
def announceLeader():
    global LeaderState, LeaderId, LeaderElectionStartTime
    
    LeaderState = BLE_STATE_STABLE
    LeaderId = NodeId
    LeaderElectionStartTime = None
    
    for id in [x for x in range(1, TotalNodes + 1) if x != NodeId]:
        sendToNode(BLE_COORDINATOR % (NodeId), id, TCP)
        

# Ask all other nodes for the proposal number
def requestProposalNumber():
    global ProposalNumberState
    
    ProposalNumberState = PRN_STATE_INPROGRESS
    for id in [x for x in range(1, TotalNodes + 1) if x != NodeId]:
        sendToNode(MSG_REQUEST_PN % (NodeId), id, TCP)
        
        
# Start patching our log
def patchLog():
    global PaxosState, PatchQueue, MyLog, PaxosIsPatching
    
    # Start at eventNum (log position) 0 and work our way up
    # If we don't have an event in our log for the index, execute Synod algorithm for it
    #   - Wait up to 3 seconds for an event with that index to be added into our log (BLOCK) -> As in wait for a PXS_COMMIT response from any node
    #   - If no event is added to the log after after the time limit, then we assume that we're up to date
    # If we do, then increment index and repeat the same thing
    
    PaxosState = PXS_STATE_PATCHING
    PaxosIsPatching = True
    index = 0
    
    while True:
        if not MyLog.containsEventIndex(index):
            startTime = time.time()
            eventPatched = False
            
            for id in [x for x in range(1, TotalNodes + 1) if x != NodeId]:
                sendToNode(PXS_PROBE + "|" + str(index) + "|" + str(NodeId), id, UDP)
            
            while time.time() < startTime + PXS_PATCH_TIMEOUT:
                if MyLog.containsEventIndex(index):
                    eventPatched = True
                    break
                time.sleep(0.2)
                
            if not eventPatched:
                break
        index += 1
    
    PaxosIsPatching = False
    PaxosState = PXS_STATE_READY
        

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
    global PaxosForwardingTracker
    
    name = request.form.get("name", None)
    day = request.form.get("day", None)
    start_time = request.form.get("start_time", None)
    end_time = request.form.get("end_time", None)
    participants = []
    
    if request.form.get("alan", None) != None:   participants.append(1)
    if request.form.get("bob", None) != None:    participants.append(2)
    if request.form.get("carl", None) != None:   participants.append(3)
    if request.form.get("david", None) != None:  participants.append(4)
    if request.form.get("earl", None) != None:   participants.append(5)
       
       
    # Process data 
    appointment = Appointment(name, int(day), int(start_time), int(end_time), participants) 
    
    # Should not perform local conflict checking here because the local replica could be outdated
    # MyCalendar.conflict(appointment):
    
    # Create new insert event
    pEvent = Event(0 if len(MyLog.events) == 0 else MyLog.events[-1].eventNum + 1, OP_INSERT, appointment, NodeId)
    pEventString = pEvent.serialize()
    
    # Block until the leader is established
    while LeaderState != BLE_STATE_STABLE:
        time.sleep(0.1)
        
    print "[DSP2] Notifying the leader %d about the pending event" % (LeaderId)
    sendToNode(PXS_FORWARD + "|" + pEventString + "|" + str(NodeId), LeaderId, UDP)
    
    # Track this new event
    PaxosForwardingTracker[pEventString] = time.time()
    
    return redirect("/", code = 302)
    

# Deleting an appointment
@WebApp.route("/delete", methods=["POST"])
def web_delete():
    global PaxosForwardingTracker
    
    appointmentName = request.form.get("id", "").lower().replace("___", " ")
    appointmentQuery = [app for app in MyCalendar.appointments if app.name.lower() == appointmentName]
    
    if len(appointmentQuery) > 0:
        appointment = appointmentQuery[0]
        pEvent = Event(MyLog.events[-1].eventNum + 1, OP_DELETE, appointment, NodeId)
        pEventString = pEvent.serialize()
        
        while LeaderState != BLE_STATE_STABLE:
            time.sleep(0.1)
            
        print "[DSP2] Notifying the leader %d about the pending event" % (LeaderId)
        sendToNode(PXS_FORWARD + "|" + pEventString + "|" + str(NodeId), LeaderId, UDP)
        
        # Track this new event
        PaxosForwardingTracker[pEventString] = time.time()
        
    return redirect("/", code = 302)
    
    
# Force reload on all instances
@WebApp.route("/force_reload", methods=["GET"])
def web_force_reload():
    for id in [x for x in range(1, TotalNodes + 1) if x != NodeId]:
        sendToNode(MSG_RELOAD % (NodeId), id, TCP)
    return redirect("/", code = 302)
    
    
# Returns whether the page needs to be reloaded
@WebApp.route("/ask_reload", methods=["POST"])
def web_ask_reload():
    global Reload
    
    if Reload:
        Reload = False
        return "1"
    return "0"

    
# Page not found
@WebApp.errorhandler(404)
def web_notfound(error):
    return "Page not found"


# Flask web server thread
def webServer():
    WebApp.run(host = "0.0.0.0", port = WebPort, debug = True, use_reloader = False)
    
    
# TCP server thread
def tcpServer():
    global LeaderId, LeaderState, LeaderElectionStartTime, Reload, MyState
    
    listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listener.bind(("0.0.0.0", TcpPort))
    listener.listen(TotalNodes)
    
    while True:
        (connection, address) = listener.accept()
        data = connection.recv(BUFFER_SIZE)
        if len(data) == 0:
            continue
            
        pieces = data.split("-")
        if len(pieces) < 2:
            continue
            
        # If we receive RELOAD, then set the Reload flag to true (unrelated to the leader election algorithm)
        if pieces[0] == MSG_RELOAD_BASE:
            Reload = True
            continue
            
        # If we receive BROADCAST_PROPOSAL_NUMBER, then update our record of the highest proposal number in stable storage
        # However, we do not need to call save() after updating, as the UDP server thread will automatically handle this for us
        if pieces[0] == MSG_BROADCAST_PN_BASE:
            if len(pieces) == 3 and (MyState.getHighestProposal() == None or int(pieces[2]) > MyState.getHighestProposal()):
                MyState.setHighestProposal(int(pieces[2]))
                ProposalNumberState = PRN_STATE_STABLE
                
        # If we receive REQUEST_PROPOSAL_NUMBER, then send our copy of the highest proposal number to the sender
        if pieces[0] == MSG_REQUEST_PN_BASE:
            sendToNode(MSG_BROADCAST_PN % (NodeId, MyState.getHighestProposal()), int(pieces[1]), TCP)
            
        # Log the message
        try:
            print "[DSP2] Received '%s' from node %s" % (data, pieces[1])
        except:
            pass
             
        # If another node started an election and we have a higher ID, then send OK back to it and start another election ourselves
        if pieces[0] == "ELECTION":
            senderID = int(pieces[1])
            if NodeId > senderID:
                sendToNode(BLE_OK % (NodeId), senderID, TCP)
                startElection()
            
        # If we sent ELECTION and received OK, then we are not the leader and we don't know who the leader is yet
        elif pieces[0] == "OK":
            if LeaderState == BLE_STATE_INPROGRESS:
                LeaderState = BLE_STATE_UNKNOWN
                LeaderElectionStartTime = None
        
        # If we received COORDINATOR, then we now know who the new leader is
        elif pieces[0] == "COORDINATOR":
            LeaderState = BLE_STATE_STABLE
            LeaderId = int(pieces[1])
        
        
# UDP server thread
def udpServer():
    global LeaderId, NodeId, PaxosACKNum, PaxosState, PaxosTimer, PaxosPromiseResponses, PaxosPendingEvent, PaxosEventIndex, PaxosForwardingQueue, PaxosForwardingTracker
    global MyCalendar, MyLog, MyState, PatchQueue, ReadyQueue
    
    listener = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    listener.bind(("0.0.0.0", UdpPort))
    
    while True:
            
        # Block until we get another message
        (data, address) = listener.recvfrom(8192)
        
        # Some basic error checking
        if data == None or len(data) == 0:
            continue
            
        pieces = data.split("|")
        if len(pieces) < 2:
            continue
            
        # Log the message
        try:
            print "[DSP2] Received '%s' from node %d" % (data, int(pieces[-1]))
        except:
            pass
                   
        # Are we the leader? (proposer)
        if NodeId == LeaderId:
            
            # Did another proposer forward an event to us?
            if pieces[0] == PXS_FORWARD:
                pEvent = Event(None, None, None, None)
                pEvent.unserialize(pieces[1])
                
                # Check if this appointment causes a conflict in our calendar (which should be up to date if we're in the ready state)
                # If it does cause a conflict, then respond with a CONFLICT acknowledgement. If not, then respond with regular ACK and add to ready queue.
                if pEvent.opType == OP_INSERT and MyCalendar.conflict(pEvent.appointment):
                    sendToNode(PXS_FORWARD_CONFLICT + "|" + pieces[1] + "|" + str(NodeId), int(pieces[2]), UDP)
                else:
                    sendToNode(PXS_FORWARD_ACK + "|" + pieces[1] + "|" + str(NodeId), int(pieces[2]), UDP)
                    ReadyQueue.append(pEvent)
            
            # Are we waiting for "promise" responses from a proposal?
            if PaxosState == PXS_STATE_PROPOSING:
                if pieces[0] == PXS_PROMISE:
                    if pieces[1] == "NULL" and pieces[2] == "NULL":
                        PaxosPromiseResponses.append((None, None))
                    else:
                        PaxosPromiseResponses.append((int(pieces[1]), pieces[2]))

            # Are we waiting for "ack" responses from an accept request?
            elif PaxosState == PXS_STATE_ACCEPTING:
                if pieces[0] == PXS_ACK:
                    PaxosACKNum += 1

        
        # ------ Acceptor logic ------
        
        # Did we receive a prepare request from the proposer?
        if pieces[0] == PXS_PREPARE:
            pNumber = int(pieces[1])
            pEvent = Event(None, None, None, None)
            pEvent.unserialize(pieces[2])
            n = pEvent.eventNum
            
            if n == None:
                continue
            
            if MyState.getMaxPrepare(n) == None or pNumber > MyState.getMaxPrepare(n):
                MyState.setMaxPrepare(n, pNumber)
                
                if MyState.getAccNum(n) > 0 and (MyState.getAccVal(n) == None or len(MyState.getAccVal(n)) > 0):
                    sendToNode(PXS_PROMISE + "|" + str(MyState.getAccNum(n)) + "|" + MyState.getAccVal(n) + "|" + str(NodeId), LeaderId, UDP)
                else:
                    sendToNode(PXS_PROMISE + "|NULL|NULL|" + str(NodeId), LeaderId, UDP)
            
        # Did we receive an accept request from the proposer?
        if pieces[0] == PXS_ACCEPT:
            pNumber = int(pieces[1])
            pValue = pieces[2]
            pEvent = Event(None, None, None, None)
            pEvent.unserialize(pValue)
            n = pEvent.eventNum
            
            if n == None:
                continue
            
            if MyState.getMaxPrepare(n) == None or pNumber >= MyState.getMaxPrepare(n):
                MyState.setAccNum(n, pNumber)
                MyState.setAccVal(n, pValue)
                sendToNode(PXS_ACK + "|" + str(pNumber) + "|" + pValue + "|" + str(NodeId), LeaderId, UDP)
                
        # Has an event in the log been committed/chosen?
        if pieces[0] == PXS_COMMIT:
            pEvent = Event(None, None, None, None)
            pEvent.unserialize(pieces[1])
            
            # If the event is already in the log (in the case of the leader patching its log), then don't commit anything
            # Or if the event index is invalid, then don't add it in the log either
            # If it's a new event, then update the log accordingly
            if pEvent.eventNum == None or pEvent.eventNum < 0 or MyLog.containsEventIndex(pEvent.eventNum):
                continue
            
            # Add event to log
            MyLog.addEvent(pEvent)
            
            # Add appointment to calendar
            if pEvent.opType == OP_INSERT:
                MyCalendar.add(pEvent.appointment)
            elif pEvent.opType == OP_DELETE:
                MyCalendar.remove(pEvent.appointment)
                
        # ------ Acceptor logic ------
        
        
        # ------ General Proposer logic ------
        
        # Did we receive an acknowledgement (either positive or conflict) from leader in response to a forwarded event?
        if pieces[0] == PXS_FORWARD_ACK or pieces[0] == PXS_FORWARD_CONFLICT:
            del PaxosForwardingTracker[pieces[1]]
            
            if pieces[0] == PXS_FORWARD_CONFLICT:
                ev = Event(None, None, None, None)
                ev.unserialize(pieces[1])
                MyAlerts.append("Event '%s' caused a conflict and was not added to the calendar" % (ev.appointment.name))
        
        # Did we receive a probe request asking if we have a commmited/chosen copy of an event in our log?
        if pieces[0] == PXS_PROBE and MyLog.containsEventIndex(int(pieces[1])):
            event = [ev for ev in MyLog.events if ev.eventNum == int(pieces[1])][0]
            sendToNode(PXS_COMMIT + "|" + event.serialize() + "|" + str(NodeId), int(pieces[2]), UDP)
            
        # ------ General Proposer logic ------
        
        MyState.save()
        time.sleep(0.1)
        
        
# Thread to start the Synod algorithm (only sending the initial prepare messages)
def synodStarter():
    global LeaderId, NodeId, PaxosACKNum, PaxosState, PaxosTimer, PaxosPromiseResponses, PaxosPendingEvent, PaxosEventIndex, PaxosForwardingQueue, PaxosForwardingTracker
    global MyCalendar, MyLog, MyState, PatchQueue, ReadyQueue
    
    
    while True:
        
        # Next event to be processed
        pEvent = None
    
        # Are we currently patching our local log?
        #if PaxosState == PXS_STATE_PATCHING and len(PatchQueue) > 0:
        #    pEvent = PatchQueue.pop(0)
    
        # Or are we ready to process new log entries?
        if PaxosState == PXS_STATE_READY and len(ReadyQueue) > 0:
            pEvent = ReadyQueue.pop(0)
        
        # If there is a next event to be processed, then run the Synod algorithm on it
        if pEvent != None:
            # Generate a new proposal number
            proposalNumber = MyState.highestProposal + 1
            MyState.setHighestProposal(proposalNumber)
    
            # Broadcast this proposal number to all of the other nodes
            for id in [x for x in range(1, TotalNodes + 1) if x != NodeId]:
                sendToNode(MSG_BROADCAST_PN % (NodeId, proposalNumber), id, TCP)
    
            # If the index position of the event is set, this means that we're patching the log for an existing event
            # If it is not set, then we're creating a new event to add to the log, so we need to set a new eventNum that's higher than all the others
            if pEvent.eventNum == None:
                if len(MyLog.events) > 0:
                    if pEvent.eventNum == None or pEvent.eventNum < MyLog.events[-1].eventNum:
                        pEvent.eventNum = MyLog.events[-1].eventNum + 1
                else:
                    pEvent.eventNum = 0
    
            # Start a new proposal, update state accordingly
            resetPaxosState()
            PaxosTimer = time.time()
            PaxosPendingEvent = pEvent
            PaxosEventIndex = pEvent.eventNum
            PaxosState = PXS_STATE_PROPOSING
    
            # Send prepare request to all acceptors (including ourself)
            for id in range(1, TotalNodes + 1):
                sendToNode(PXS_PREPARE + "|" + str(proposalNumber) + "|" + PaxosPendingEvent.serialize() + "|" + str(NodeId), id, UDP)
                
    time.sleep(0.5)


# Thread to check for election response timeouts and to check if the leader is still alive
def leaderState():
    global LeaderElectionStartTime, LeaderState, LeaderId, LeaderLastHeartbeat
    
    while True:
        electStart = LeaderElectionStartTime if LeaderElectionStartTime != None else 0
        
        # Send a heartbeat to the leader (periodically) to see if it is still up. If it isn't, then start a new election
        if LeaderState == BLE_STATE_STABLE and NodeId != LeaderId and (LeaderLastHeartbeat == None or time.time() > LeaderLastHeartbeat + BLE_HEARTBEAT_FREQUENCY):
            LeaderLastHeartbeat = time.time()
            if not sendToNode(BLE_HEARTBEAT, LeaderId, TCP, True):
                print "[DSP2] Leader node %d has gone down, starting new election" % (LeaderId)
                startElection()
            
        # If we started an election and have not received any OK responses with the time limit, then we are the new leader
        if LeaderState == BLE_STATE_INPROGRESS and LeaderElectionStartTime != None and time.time() > LeaderElectionStartTime + BLE_RESPONSE_TIMEOUT:
            print "[DSP2] This node (%d) is the new leader" % (NodeId)
            announceLeader()
            
        # Sleep for 100 milliseconds to avoid making these computations too CPU intensive
        time.sleep(0.1)
        

# Thread for proposer to perform paxos algorithm finite state machine
def proposerState():
    global LeaderId, NodeId, PaxosACKNum, PaxosState, PaxosTimer, PaxosPromiseResponses, PaxosPendingEvent, PaxosEventIndex, PaxosForwardingTracker, PaxosForwardingQueue
    global MyCalendar, MyLog, MyState
    
    while True:
        
        # Are we the leader?
        if NodeId == LeaderId:
            
            # If we're waiting for promises and we have a majority of them, then choose the one with the highest proposal number
            if PaxosState == PXS_STATE_PROPOSING:
                if len(PaxosPromiseResponses) >= Majority:
                    PaxosState = PXS_STATE_ACCEPTING
                    PaxosTimer = time.time()
                    
                    highestNumber = None
                    highestValue = None
                    
                    # Look for the value of the response with the highest proposal number
                    for r in PaxosPromiseResponses:
                        if r[0] != None and r[0] > highestNumber:
                            highestNumber = r[0]
                            highestValue = r[1]
                            
                    # If at least one of the responses is non-null, then this becomes the new pending event that we will send an accept request for
                    if highestNumber != None:
                        PaxosPendingEvent = Event(None, None, None, None)
                        PaxosPendingEvent.unserialize(highestValue)
                        
                    # If all the responses are null but our event is invalid (eg: we have no appointment), then this is a patch request
                    # and we are at the end of the patch, so abort the Synod algorithm and do not proceed with an accept request
                    if highestNumber == None and PaxosPendingEvent.appointment == None:
                        continue
                    
                    # Send accept request to all acceptors for the chosen value from above
                    for id in range(1, TotalNodes + 1):
                        sendToNode(PXS_ACCEPT + "|" + str(MyState.highestProposal) + "|" + PaxosPendingEvent.serialize() + "|" + str(NodeId), id, UDP)
                
                # If the leader does not receive enough votes, it returns to idle state
                elif time.time() > PaxosTimer + PXS_TIMEOUT:
                    print "[DSP2] Not enough votes from members. Timeout."
                    resetPaxosState()
                    
                
            # If we're waiting for ACK responses from our accept request and have a majority, then send commit message
            elif PaxosState == PXS_STATE_ACCEPTING:
                if PaxosACKNum >= Majority:
                    for id in range(1, TotalNodes + 1):
                        sendToNode(PXS_COMMIT + "|" + PaxosPendingEvent.serialize() + "|" + str(NodeId), id, UDP)
                    resetPaxosState()
                
                elif time.time() > PaxosTimer + PXS_TIMEOUT:
                    print "[DSP2] Not enough commits from members. Timeout."
                    resetPaxosState()
        
        
        # General Proposer Logic
        
        # If we have not received an ACK/CONFLICT response to a forwarded event, then add it to the forwarding queue
        timedOutEvents = []
        
        for eventDataString, timeSent in PaxosForwardingTracker.iteritems():
            if time.time() > timeSent + PXS_TIMEOUT:
                PaxosForwardingQueue.append(eventDataString)
                timedOutEvents.append(eventDataString)
        
        for eventDataString in timedOutEvents:
            del PaxosForwardingTracker[eventDataString]
                    
        
        # If there are events in the forwarding queue and the leader is back online, then re-forward the events to the leader
        # Note that once we re-forward the event, we also need to start tracking it again (in case the re-forwarded event isn't received either)
        if LeaderState == BLE_STATE_STABLE:
            for eventDataString in PaxosForwardingQueue:
                sendToNode(PXS_FORWARD + "|" + eventDataString + "|" + str(NodeId), LeaderId, UDP)
                PaxosForwardingTracker[eventDataString] = time.time()
            PaxosForwardingQueue = []
        
        # End General Proposer Logic
        
        time.sleep(0.2)
    

# Program execution officially starts here
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "Usage: sudo python server.py [nodeId] [local?]"
        sys.exit(1)
          
    # Initialize application start based on command line arguments
    NodeId = int(sys.argv[1])
    LogFile = LogFile % (NodeId)
    StateFile = StateFile % (NodeId)
    
    if NodeId < 1 or NodeId > TotalNodes:
        print "Invalid node ID, cannot start server!"
        sys.exit(1)
        
    if len(sys.argv) >= 3 and sys.argv[2] == "local":
        Localhost = True
        WebPort = NodeData[NodeId][3][0]
        TcpPort = NodeData[NodeId][3][1]
        UdpPort = NodeData[NodeId][3][2]
    else:
        WebPort = NodeData[NodeId][4][0]
        TcpPort = NodeData[NodeId][4][1]
        UdpPort = NodeData[NodeId][4][2]
        
        
    # Some debug information
    print "[DSP2] Starting application with nodeId = %d" % (NodeId)
    print "[DSP2] Web server running on port %d, TCP running on port %d, UDP running on port %d" % (WebPort, TcpPort, UdpPort)
    
    # More initialization
    MyLog = Log()
    MyCalendar = Calendar()
    MyState = PaxosStorage()
    PaxosState = PXS_STATE_PATCHING
    ProposalNumberState = PRN_STATE_UNKNOWN
    
    # Thread #1: Web server
    WebThread  = threading.Thread(target = webServer)
    WebThread.daemon = True
    WebThread.start()
    
    # Thread #2: TCP server (Leader election)
    TcpThread = threading.Thread(target = tcpServer)
    TcpThread.daemon = True
    TcpThread.start()
    
    # Thread #3: UDP server (Paxos algorithm)
    UdpThread = threading.Thread(target = udpServer)
    UdpThread.daemon = True
    UdpThread.start()
    
    # Thread #4: Leader state machine
    LeaderThread = threading.Thread(target = leaderState)
    LeaderThread.daemon = True
    LeaderThread.start()
    
    # Thread #5: Proposer state machine
    ProposerThread = threading.Thread(target = proposerState)
    ProposerThread.daemon = True
    ProposerThread.start()
    
    # Thread #6: Synod algorithm starter
    SynodStarterThread = threading.Thread(target = synodStarter)
    SynodStarterThread.daemon = True
    SynodStarterThread.start()
    
    # Disable logging of HTTP requests
    log = logging.getLogger("werkzeug")
    log.setLevel(logging.ERROR)
    
    # Start an election
    startElection()
    
    # Ask all other nodes what their highest proposal number is
    requestProposalNumber()
    
    # Start patching the log
    patchLog()
    
    # Infinite loop
    while threading.active_count() > 0:
        time.sleep(0.1)