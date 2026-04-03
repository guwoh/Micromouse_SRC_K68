% Data taking

data = readtable('Data1.csv');
time = data{:,1};
ul = data{:,"pwm"};
ur = ul;
yl = data{:,"posLeft"};
yr = data{:,"posRight"};

dt = mean(diff(time));

%vl = smoothdata(gradient(yl, dt));
%vr = smoothdata(gradient(yr, dt));
vl = gradient(yl, dt);
vr = gradient(yr, dt);

t_v = time;

al = gradient(vl, dt);
ar = gradient(vr, dt);

t_a = time;

%ul = ul(1:end-2);
%ur = ur(1:end-2);

figure;
subplot(2,1,1);
plot(time, yl); title('Position');

subplot(2,1,2);
plot(t_v, vl); title('Velocity');

% Model calculate



A = [vl, al, ones(length(al),1)];
b = ul;

x = A \ b;

Kv = x(1);
Ka = x(2);
offset = x(3);

u_est = Kv*vl + Ka*al + offset;

figure;
plot(b); hold on;
plot(u_est);
legend('Real PWM','Estimated PWM');

v_final = mean(vl(1400:2000));
v_63 = 0.63 * v_final;

idx = find(vl(1000:2000) >= v_63, 1);
tau = time(idx);


L = 76;
omega = (vr - vl) / L;

alpha = gradient(omega, dt); % gia tốc góc

figure;
subplot(2,1,1);
plot(time, alpha); title('Gia tốc góc');


subplot(2,1,2);
plot(time, omega); title('Vận tốc góc');