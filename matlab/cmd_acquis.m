%% Interface série commande 8 bits, définie par moi, pour moi
% fonctionne avec le programme arduino 'interfaceSerie' ci-joint
close all
clc

s = serial('COM4');
set(s,'BaudRate',115200)
s.InputBufferSize = 65536;
fopen(s);

pause(1);

if(s.bytesAvailable)
temp= fread(s,s.bytesAvailable);      %% Flushing buffer
end

%% GENERATE_FIRST_PWM - Start PWM 25%
fwrite(s,bin2dec('00000001'));     

%% ACQUIRE Start PWM 50%
fwrite(s,bin2dec('00000010'));
%% SEND_DATA
fwrite(s,bin2dec('00000100'));

while(~s.bytesAvailable)        %% wait till it comes
    end  %% wait for bits...

vec1 = fread(s,1000,'uint16');  %% just readit
fwrite(s,bin2dec('00000100'));

while(~s.bytesAvailable)        %% wait till it comes
    end  %% wait for bits...
vec2 = fread(s,1000,'uint16');
fwrite(s,bin2dec('00000100'));

while(~s.bytesAvailable)        %% wait till it comes
    end  %% wait for bits...
vec3 = fread(s,1000,'uint16');
fwrite(s,bin2dec('00000100'));

while(~s.bytesAvailable)        %% wait till it comes
    end  %% wait for bits...
vec4 = fread(s,1000,'uint16');
%vec = (vec/32000)*500;
figure
plot(vec1, vec2, vec3, vec4)
%% ferme ca
fclose(s);

%% 
X = [1:1:1000];
plot(X, vec1, X, vec2, X, vec3);
%% plo
figure
plot(vecteur)