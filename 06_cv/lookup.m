load ntc.csv
tem = ntc(1:156,1);
ADC_val = 1024*(ntc(1:156, 2)./(ntc(1:156, 2)+10))
p = polyfit(ADC_val, tem, 10);
ad2 = 0:1023; 
t2 = round(polyval(p, ad2), 1);
plot(ADC_val, tem, 'o')
hold on;
plot(ad2, t2)

dlmwrite('data.dlm', t2*10, ',');