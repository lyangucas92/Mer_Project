%%
% clear
clc;
clear;
close all;

%%
% ����ͼ��
I1  = imread('Image000.bmp');
I1  = rgb2gray(I1);clc
figure;
imshow(I1); 
title('Original Images');

%%
% ��˹�˲���
PQ=paddedsize(size(I1));
D0=0.05*PQ(2);

H_gaussian=lpfilter('gaussian',PQ(1),PQ(2),D0);    
g_gaussian=dftfilt(I1,H_gaussian);
g_gaussian=gscale(g_gaussian);

% ��ʾ
figure;
imshowpair(fftshift(H_gaussian), g_gaussian, 'montage'); 
title('Gaussian Images');


% ��ֵ�˲�
I1 = medfilt2(g_gaussian,[3 3]); %����3 �� 3ģ�����ֵ�˲�
% ��ʾ
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
% sobel���ؼ��
ecanny_bwI1 = edge(bw_I1,'canny');
figure;
imshow(ecanny_bwI1); 
title('sobel bwImages');


%%
% ������
strel_mat = strel('square',7);
close_I1 = imclose(ecanny_bwI1,strel_mat);
figure;
imshow(close_I1); 
title('close bwImages');
   
 

