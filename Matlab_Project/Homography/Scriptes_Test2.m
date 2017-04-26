%%
% ��������ʵ������ͼ����ȡ

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
% Ostu��ֵ��
level_I1 = graythresh(I1);
im = im2bw(I1,level_I1);
figure;
imshow(im); 
title('bw Images');


%%
% ��ͨ��ĸ���
[L,num] = bwlabel(im,4);

%%
% ������
[M,N]=size(im);
B=bwboundaries(im);%Ѱ������,BΪ�洢������Ϣ�Ľṹ��
im1=zeros(M,N);
for k=1:length(B)
    boundary=B{k};%��k���������洢��������������
    
    %��ע��k��������������λ��Ϊ1
    for l=1:length(boundary)
        im1(boundary(l,1),boundary(l,2))=1;
    end  
end
figure(2);imshow(im1);title('����ͼ')%��������ͼ

%%
% ������
[nr,nc]=size(B{1});
Code=size(1,nr);

for i=1:nr     
    %x=-2,-1,0,1,2
    %y=-2,-1,0,1,2
    if i==1
        x=B{1}(i,1)-B{1}(nr-1,1);   
        y=B{1}(i,2)-B{1}(nr-1,2);   
    elseif i==2
        x=B{1}(i,1)-B{1}(nr,1);   
        y=B{1}(i,2)-B{1}(nr,2);   
    elseif i~=1
        x=B{1}(i,1)-B{1}(i-2,1);   
        y=B{1}(i,2)-B{1}(i-2,2);   
    end
    
    %16������
    if x==2 && y==0
        Code(i)=0;
    elseif x==2 && y==-1
        Code(i)=1;
    elseif x==2 && y==-2
        Code(i)=2;
    elseif x==1 && y==-2
        Code(i)=3;
    elseif x==0 && y==-2
        Code(i)=4;
    elseif x==-1 && y==-2
        Code(i)=5;
    elseif x==-2 && y==-2
        Code(i)=6;
    elseif x==-2 && y==-1
        Code(i)=7;
     elseif x==-2 && y==0
        Code(i)=8;
    elseif x==-2 && y==1
        Code(i)=9; 
    elseif x==-2 && y==2
        Code(i)=10;
    elseif x==-1 && y==2
        Code(i)=11;
    elseif x==0 && y==2
        Code(i)=12; 
    elseif x==1 && y==2
        Code(i)=13;  
    elseif x==2 && y==2
        Code(i)=14;   
    elseif x==2 && y==1
        Code(i)=15;              
    end   
end