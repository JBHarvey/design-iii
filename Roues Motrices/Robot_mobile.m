function Robot_mobile(block)
%ROBOT_MOBILE Une S-Function Level-2
%   Fonction simulant un robot mobile (differential drive)
%
%   Basé sur http://www.kirp.chtf.stuba.sk/pc11/data/abstracts/023.html
%
%   2015-11-27 André Desbiens
setup(block);

% end function


function setup(block)

% Register number of ports
block.NumInputPorts  = 2;
block.NumOutputPorts = 7;

% Setup port properties to be inherited or dynamic
block.SetPreCompInpPortInfoToDynamic;
block.SetPreCompOutPortInfoToDynamic;

% Override input port properties
for i = 1:block.NumInputPorts
    block.InputPort(i).Dimensions  = 1;
    block.InputPort(i).DatatypeID  = 0;  % double
    block.InputPort(i).Complexity  = 'Real';
    block.InputPort(i).DirectFeedthrough = false;
    block.InputPort(i).SamplingMode = 'Sample';
end

% Override output port properties
for i = 1:block.NumOutputPorts
    block.OutputPort(i).Dimensions  = 1;
    block.OutputPort(i).DatatypeID  = 0; % double
    block.OutputPort(i).Complexity  = 'Real';
    block.OutputPort(i).SamplingMode = 'Sample';
end

% Register parameters
block.NumDialogPrms = 0;

% Set up the continuous states
block.NumContStates = 11;

% Register sample times
block.SampleTimes = [0 0]; 

% Specify the block simStateCompliance. The allowed values are:
block.SimStateCompliance = 'DefaultSimState';

% Register methods
block.RegBlockMethod('InitializeConditions', @InitializeConditions);
block.RegBlockMethod('Outputs', @Outputs);
block.RegBlockMethod('Derivatives', @Derivatives);

%end setup


function InitializeConditions(block)

block.ContStates.Data(1) = 0;
block.ContStates.Data(2) = 0;
block.ContStates.Data(3) = 0;
block.ContStates.Data(4) = 0;
block.ContStates.Data(5) = 0;
block.ContStates.Data(6) = 0;
block.ContStates.Data(7) = 0;

% Limites de la figure d'animation
block.ContStates.Data(8) = -5; % xmin;
block.ContStates.Data(9) = 5; % xmax;
block.ContStates.Data(10) = -5; % ymin;
block.ContStates.Data(11) = 5; % ymax;

% Création de la figure d'animation
figure(1)
clf;
hold on;
title('POSITION RÉELLE DU ROBOT');
axis([block.ContStates.Data(8) block.ContStates.Data(9) block.ContStates.Data(10) block.ContStates.Data(11)]);
grid;

%end InitializeConditions


function Outputs(block)

% bruit sur les mesures
bruit=1; %0 : pas de bruit de mesure   1: avec bruit de mesure
sd_theta = bruit*2*pi/100*randn;    % random standard deviation of measurement noise on theta [rad]
sd_x = bruit*0.015*randn;            % random standard deviation of measurement noise on x [m]
sd_y = bruit*0.015*randn;            % random standard deviation of measurement noise on y [m]
sd_omegaL = bruit*0.5*randn;        % random standard deviation of measurement noise on omegaL [rad/s]
sd_omegaR = bruit*0.5*randn;        % random standard deviation of measurement noise on omegaR [rad/s]

% mesures
block.OutputPort(1).Data = block.ContStates.Data(1) + sd_theta*randn; % robot heading (counter clockwise from the x-axis when viewed from top) [rad]
block.OutputPort(2).Data = block.ContStates.Data(2) + sd_x*randn; % robot position (x coordinate) [m]
block.OutputPort(3).Data = block.ContStates.Data(3) + sd_y*randn; % robot position (y coordinate) [m]
block.OutputPort(6).Data = block.ContStates.Data(6) + sd_omegaL*randn; % left wheel angular speed [rad/s]
block.OutputPort(7).Data = block.ContStates.Data(7) + sd_omegaR*randn; % right wheel angular speed [rad/s]

% autres états
block.OutputPort(4).Data = block.ContStates.Data(4) + sd_x*randn; % i_L
block.OutputPort(5).Data = block.ContStates.Data(5) + sd_y*randn; % i_R

% animation
cla
a=0.5;
b=0.5;
c=0.15;
e=sqrt(b^2+c^2);
nu=pi/2-asin(b/e);
x=block.ContStates.Data(2);
y=block.ContStates.Data(3);
theta=block.ContStates.Data(1);
x1=x+a*cos(theta);
y1=y+a*sin(theta);
x2=x-e*cos(nu-theta);
y2=y+e*sin(nu-theta);
x3=x+e*sin(nu-pi/2+theta);
y3=y-e*cos(nu-pi/2+theta);
robotxcoord=[x1 x2 x3];
robotycoord=[y1 y2 y3];
fill(robotxcoord,robotycoord,'k');
t=['t=' num2str(block.CurrentTime)];
text(block.ContStates.Data(8)+0.025*(block.ContStates.Data(9)-block.ContStates.Data(8)),block.ContStates.Data(11)-0.025*(block.ContStates.Data(11)-block.ContStates.Data(10)),t);

%end Outputs


function Derivatives(block)

% Paramètres
lL=0.05;
lR=0.05;
lT=0.02;
r=0.05;
m=1.25;
kv=0.1;
JT=0.55;
kw=1.35;
RR=2;
RL=1.05*2;
LL=0.05;
LR=1.05*0.05;
K=0.1;
Rz=0.2;
J=0.025;
kr=0.002;
pG=25;
usat = 15;       % saturation of motor input voltage between usat and -usat

rG=r/pG;
JB=JT+m*lT^2;
aL=kr+(kv*lR*rG^2)/(lL+lR);
aR=kr+(kv*lL*rG^2)/(lL+lR);
bL=J+(m*lR*rG^2)/(lL+lR);
bR=J+(m*lL*rG^2)/(lL+lR);
cL=kr*lL+(kw*rG^2)/(lL+lR);
cR=kr*lR+(kw*rG^2)/(lL+lR);
dL=J*lL+(JB*rG^2)/(lL+lR);
dR=J*lR+(JB*rG^2)/(lL+lR);

% Entrées
uL = sat(block.InputPort(1).Data,usat); % left motor input voltage [V]
uR = sat(block.InputPort(2).Data,usat); % right motor input voltage [V]

% États
theta  = block.ContStates.Data(1);
x  = block.ContStates.Data(2);
y  = block.ContStates.Data(3);
iL  = block.ContStates.Data(4);
iR  = block.ContStates.Data(5);
omegaL  = block.ContStates.Data(6);
omegaR  = block.ContStates.Data(7);

% Dérivées des états
block.Derivatives.Data(1) = rG*(-omegaL+omegaR)/(lL+lR);
v=rG*(lR*omegaL+lL*omegaR)/(lL+lR);
block.Derivatives.Data(2) = v*cos(theta);
block.Derivatives.Data(3) = v*sin(theta);
block.Derivatives.Data(4) = (uL-K*omegaL-(RL+Rz)*iL-Rz*iR)/LL;
block.Derivatives.Data(5) = (uR-K*omegaR-(RR+Rz)*iR-Rz*iL)/LR;
block.Derivatives.Data(6) = (1/(bL*dR+bR*dL))*(dR*(K*(iL+iR)-aL*omegaL-aR*omegaR)-bR*(K*(-lL*iL+lR*iR)+cL*omegaL-cR*omegaR));
block.Derivatives.Data(7) = (1/(bL*dR+bR*dL))*(dL*(K*(iL+iR)-aL*omegaL-aR*omegaR)+bL*(K*(-lL*iL+lR*iR)+cL*omegaL-cR*omegaR));

%end Derivatives


function y=sat(u,usat);

if u>usat
    y=usat;
elseif u<-usat 
    y=--usat;
else 
    y=u;
end

%end sat
