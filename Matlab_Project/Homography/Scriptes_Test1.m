%%
% clear
clc;
clear;
close all;

%%
% 导入图像
I1  = imread('Image000.bmp');
I1  = rgb2gray(I1);clc
figure;
imshow(I1); 
title('Original Images');

%%
% 高斯滤波器
PQ=paddedsize(size(I1));
D0=0.05*PQ(2);

H_gaussian=lpfilter('gaussian',PQ(1),PQ(2),D0);    
g_gaussian=dftfilt(I1,H_gaussian);
g_gaussian=gscale(g_gaussian);

% 显示
figure;
imshowpair(fftshift(H_gaussian), g_gaussian, 'montage'); 
title('Gaussian Images');


% 中值滤波
I1 = medfilt2(g_gaussian,[3 3]); %进行3 × 3模板的中值滤波
% 显示
figure;
imshow(I1);
title('Medien Images');


%%
% Ostu
level_I1 = graythresh(I1);
bw_I1 = im2bw(I1,level_I1);
figure;
imshow(bw_I1); 
title('bw Images');

%%
% sobel边沿检测
ecanny_bwI1 = edge(bw_I1,'canny');
figure;
imshow(ecanny_bwI1); 
title('sobel bwImages');


%%
% 开运算
strel_mat = strel('square',7);
close_I1 = imclose(ecanny_bwI1,strel_mat);
figure;
imshow(close_I1); 
title('close bwImages');
   
 

