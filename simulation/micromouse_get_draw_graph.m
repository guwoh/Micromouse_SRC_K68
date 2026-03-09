figure;
y1 = load("data150pwm.txt");
y2 = load("data200pwm.txt");
y3 = load("data250pwm.txt");
x = 1:length(y1);
smooth_y1 = movmean(y1, 1);
smooth_y2 = movmean(y2, 1);
smooth_y3 = movmean(y3, 1);
plot(x, smooth_y1, '-r', 'LineWidth', 2);
hold on 
plot(x, smooth_y2, '-b', 'LineWidth', 2);
hold on 
plot(x, smooth_y3, '-g', 'LineWidth', 2);
hold off

xlabel('ms');
ylabel('encoder count/ms');
title('Số encoder count/ms trong 1 giây');
legend("pwm200", "pwm250", "pwm300");
grid on;

y_max1 = 41;
threshold = 0.63 * y_max1;
idx = find(y1 >= threshold, 1); % chỉ số đầu tiên thỏa mãn % 
x_at_63 = x(idx);
x_at_63

y_max2 = 57;
threshold = 0.63 * y_max2;
idx = find(y2 >= threshold, 1); % chỉ số đầu tiên thỏa mãn % 
x_at_63 = x(idx);
x_at_63

y_max3 = 67;
threshold = 0.63 * y_max3;
idx = find(y3 >= threshold, 1); % chỉ số đầu tiên thỏa mãn % 
x_at_63 = x(idx);
x_at_63



figure;
pwm = [150 200 250];
enc_cnt = [30 50 62];

% Hồi quy tuyến tính: y ≈ a*x + b
p = polyfit(pwm, enc_cnt, 1);   % bậc 1 => đường thẳng
y_fit = polyval(p, pwm);

p = polyfit(pwm, enc_cnt, 1);

a = p(1);
b = p(2);

fprintf('Phương trình đường thẳng: y = %.4f*x + %.4f\n', a, b);
eq_str = sprintf('y = %.2f x + %.2f', a, b); 


plot(pwm, enc_cnt, 'o');        % vẽ các điểm
hold on;
plot(pwm, y_fit, '-r');   % vẽ đường thẳng xấp xỉ
text(mean(pwm), mean(enc_cnt), eq_str, 'FontSize', 12, 'Color','r', 'BackgroundColor','w');
xlabel('pwm'); 
ylabel('enc cnt'); 
 
grid on;
legend('Điểm dữ liệu','Đường thẳng fit');


T_m = 152;
K_m = 500;
