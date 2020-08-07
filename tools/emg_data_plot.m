PATH = 'D:\';
FileName = 'EMG_20200807_121110_8bits_500Hz.bin';
ftemp = fopen(fullfile(PATH,FileName), 'r', 'l');
tmp1 = fread(ftemp, 'uchar', 'l');
fclose(ftemp);

if contains(FileName,'12bits')
    tmp2 = tmp1;
    msb = tmp1(2:2:end);
    lsb = tmp1(1:2:end);
    x = msb*256+lsb;
elseif contains(FileName,'8bits')
    x = tmp1;
else
    fprintf(' Invalid Data type ');
end
Data = reshape(x, 8, []);
    
subplot(4,2,1),plot(Data(1,:));
subplot(4,2,2),plot(Data(2,:));
subplot(4,2,3),plot(Data(3,:));
subplot(4,2,4),plot(Data(4,:));
subplot(4,2,5),plot(Data(5,:));
subplot(4,2,6),plot(Data(6,:));
subplot(4,2,7),plot(Data(7,:));
subplot(4,2,8),plot(Data(8,:));
