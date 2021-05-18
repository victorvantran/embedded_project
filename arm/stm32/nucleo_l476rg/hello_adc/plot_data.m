% initialize
close all; clear all;

pkg load instrument-control
if (exist('serial') != 3)
  disp('No serial support');
endif


% serial console/object
s = serial('/dev/ttyACM0', 115200);

% open serial port
fopen(s);

% plot data
i = 100
while (1)
  data(i) = typecast(srl_read(s, 2), 'uint16');
  bitand(data(i), 4095);
  
  plot(data);
  axis([i - 100 i 0 8095]);
  
  pause(0.01);
  i = i+1;
end

% close serial port
fclose(s);












