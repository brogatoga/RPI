% Returns the number of times that the element X appears in the given list
declare
fun {Count List X}
  case List
    of H|T then
      if X == H then 1 + {Count T X}
      else {Count T X}
      end
    else 0
  end
end
% Returns whether the given expression is an atom
declare
fun {IsAtom Exp}
  true
  /*
  case Exp
    of lambda(V SubExp) then false
    [] [Exp1 Exp2] then false
    [] Atom then true
    else false
  end
  false
  */
end
% Return a list containing all FREE variables in the given expression
declare
fun {FreeVariables Exp}
  case Exp
    of lambda(V SubExp) /*andthen {IsAtom V}*/ then {List.subtract {FreeVariables SubExp} V}
    [] [Exp1 Exp2] then {List.append {FreeVariables Exp1} {FreeVariables Exp2}}
    [] Atom /*andthen {IsAtom Atom}*/ then [Atom]
    else nil
  end
end
% Return a list containing BOTH bound and free variables in the given expression
declare
fun {AllVariables Exp}   
  case Exp
    of lambda(V SubExp) then {List.append {AllVariables SubExp} [V]}
    [] [Exp1 Exp2] then {List.append {AllVariables Exp1} {AllVariables Exp2}}
    [] [Atom] then [Atom]
    [] Atom then [Atom]
    else nil
  end
end
% These three functions handle Alpha renaming
declare
fun {Rename V Database}
  if {Count Database V} > 0 then
    case V
      of Var#X then XP in XP = X+1
        {Rename Var#XP Database}
      else {Rename V#0 Database}
    end
  elseif {Count Database V} == 0 then V
  else nil
  end
end
% Alpha naming replacements
declare
fun {ReplaceFreeOccurrences X Exp Y}
  case Exp
    of [Exp1 Exp2] then [{ReplaceFreeOccurrences X Exp1 Y} {ReplaceFreeOccurrences X Exp2 Y}]
    [] lambda(V SubExp) andthen V \= X then lambda(V {ReplaceFreeOccurrences X SubExp Y})
    [] lambda(V SubExp) andthen V == X /*andthen {IsAtom V}*/ andthen SubExp == SubExp then Exp
    [] Atom andthen Atom == X then Y
    [] Atom andthen Atom \= X then Atom
    /* else nil */
    /* else Exp */
  end
end
% Actual renaming
declare
fun {AlphaRenaming Exp Database}
  case Exp
    of lambda(V SubExp) /*andthen {IsAtom V}*/ then NewName NewDatabase NewSubExp in
      NewName = {Rename V Database}
      NewDatabase = {List.append Database [NewName]}
      %NewSubExp = {ReplaceFreeOccurrences V SubExp NewName}
      NewSubExp = {ReplaceFreeOccurrences V SubExp NewName}
      lambda(NewName {AlphaRenaming NewSubExp NewDatabase})   % Recursive call
    [] [Exp1 Exp2] then [Exp1 Exp2]
    [] Atom /*andthen {IsAtom Atom}*/ then Atom
    %else Exp
    else nil
  end
end
% These functions handle Beta reduction
declare BetaReduction    % NEED FORWARD DECLARATION HERE
declare BetaReductionNormal % THIS IS FOR NORMAL MODE BETA REDUCTION
% NO FORWARD DECLARATION NEEDED FOR "BetaReduction" THOUGH
declare
fun {BetaReductionSub Exp1 Exp2}
  case Exp1
    of lambda(V SubExp) /*andthen {IsAtom V}*/ then Reduced in
       %Reduced = {Replace V SubExp Exp1}
       Reduced = {ReplaceFreeOccurrences V SubExp Exp2}
       %Reduced = {Replace V Exp1 SubExp}
       %{BetaReductionSub Reduced} no
       %{BetaReductionSub SubExp} no
       {BetaReduction Reduced}
    %[] Atom then Atom
    %[] [A B] then [A B]
    else [Exp1 Exp2]
    %else nil
  end
end
% Normal form version
%declare
%fun {BetaReductionSubNormal Exp1 Exp2}
%  case Exp1
 %   of lambda(V SubExp) /*andthen {IsAtom V}*/ then Reduced in
  %     %Reduced = {Replace V SubExp Exp1}
   %    Reduced = {ReplaceFreeOccurrences V SubExp Exp2}
    %   %Reduced = {Replace V Exp1 SubExp}
       %{BetaReductionSub Reduced} no
       %{BetaReductionSub SubExp} no
      % {BetaReductionNormal Reduced}
    %[] Atom then Atom
    %[] [A B] then [A B]
   % else [Exp1 Exp2]
    %else nil
 % end
%end
% Main routine
fun {BetaReduction Exp}
  case Exp
    of lambda(V SubExp) then lambda(V {BetaReduction SubExp})
    [] [Exp1 Exp2] then Reduced1 Reduced2 Renamed1 Renamed2 LeftVars RightVars AllVars in
    %[] [Exp1 Exp2] then Reduced1 Reduced2 Renamed1 Renamed2 AlphaVars in
         Reduced1 = {BetaReduction Exp1}
         Reduced2 = {BetaReduction Exp2}
         LeftVars = {AllVariables Reduced1}
         RightVars = {FreeVariables Reduced2}
         AllVars = {List.append LeftVars RightVars}
         Renamed1 = {AlphaRenaming Reduced1 AllVars}
         %Renamed2 = {AlphaRenaming Reduced2 AllVars}
         %Renamed2 = {AlphaRenaming Reduced2 LeftVars}
         Renamed2 = Reduced2
         %{BetaReductionSub Renamed1 Reduced2}
         {BetaReductionSub Renamed1 Renamed2}
     [] Atom /*andthen {IsAtom Atom}*/ then Atom
     %else Exp
     else nil
   end
end
% Normal form version
% Main routine
%fun {BetaReductionNormal Exp}
%  case Exp
%    of lambda(V SubExp) then lambda(V {BetaReductionNormal SubExp})
%%%%   % [] [Exp1 Exp2] then Reduced1 Reduced2 Renamed1 Renamed2 LeftVars RightVars AllVars in
 %    [] [Exp1 Exp2] then Reduced1 Renamed1 LeftVars RightVars AllVars in
%    %[] [Exp1 Exp2] then Reduced1 Reduced2 Renamed1 Renamed2 AlphaVars in
 %        Reduced1 = {BetaReduction Exp1}
 %        LeftVars = {AllVariables Reduced1}
 %        RightVars = {FreeVariables Exp2}
  %       AllVars = {List.append LeftVars RightVars}
   %      Renamed1 = {AlphaRenaming Reduced1 AllVars}
         %Renamed2 = {AlphaRenaming Reduced2 AllVars}
         %Renamed2 = {AlphaRenaming Reduced2 LeftVars}
         %{BetaReductionSub Renamed1 Reduced2}
    %     {BetaReductionSubNormal Renamed1 Exp2}
    % [] Atom /*andthen {IsAtom Atom}*/ then Atom
     %else Exp
     %else nil
   %end
%end

% This function handles the Eta reduction
declare
fun {EtaReduction Exp}
  case Exp
    of lambda(V [SubExp V]) andthen {Count {FreeVariables SubExp} V} == 0 then {EtaReduction SubExp}
    [] lambda(V SubExp) then
      lambda(V {EtaReduction SubExp})
    [] [Exp1 Exp2] then
      [{EtaReduction Exp1} {EtaReduction Exp2}]
    [] Atom then Atom
    else Exp
  end
end
% Processes the inputted expression in applicative form
declare
fun {ProcessApplicative Input}
  {EtaReduction {BetaReduction Input}}
end
% Processes the inputted expression in normal form
/*
declare
fun {ProcessNormal Input}
  {EtaReduction {BetaReductionNormal Input}}
end
*/
% Run function
declare
fun {Run Input}
  {ProcessApplicative Input}
end
/*
% RunNormal function
declare
fun {RunNormal Input}
  {ProcessNormal Input}
end
*/

% ================================================================
% ================================================================
% ================================================================
% Processes the given inputs below and evaluates it
% NOTE: You have to do Feed Buffer first before it will work with Feed Line!
{Browse {Run [lambda(x lambda(y [y x])) [y w]]}}
{Browse {Run [lambda(x lambda(y [x y])) [y w]]}}
{Browse {Run [lambda(x x) y]}}
{Browse {Run lambda(x [y x])}}
{Browse {Run [[lambda(y lambda(x [y x])) lambda(x [x x])] y]}}
{Browse {Run [[[lambda(b lambda(t lambda(e [[b t] e]))) lambda(x lambda(y x))] x] y]}}
{Browse {Run lambda(x [[lambda(x [y x]) lambda(x [z x])] x])}}
{Browse {Run [lambda(y [lambda(x lambda(y [x y])) y]) [y w]]}}
%{Browse {RunNormal [lambda(x y) [lambda(x [x x]) lambda(x [x x])]]}} % should display y
% ================================================================
% ================================================================
% ================================================================
