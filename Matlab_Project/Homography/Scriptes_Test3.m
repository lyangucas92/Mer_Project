%%
% 清理空间
clc;
clear;
close all;


%%
% 导入图像
I1  = imread('workspiece.bmp');
I1  = rgb2gray(I1);
figure;
imshow(I1); 
title('Original Images');

%%
% 中值滤波
I1 = medfilt2(I1,[6 6]); 
I1 = medfilt2(I1,[6 6]);
% 显示
figure;
imshow(I1);
title('Medien Images');

%%
% 灰度直方图
figure;
imhist(I1);  
title('灰度直方图');  

%%
% Ostu二值化
level_I1 = graythresh(I1);
Ibw = im2bw(I1,50/255);
figure;
imshow(Ibw); 

%%
% 边界提取
figure;
imshow(Ibw); 
title('bw Images');
hold on;

[L,m]=bwlabel(Ibw,8);
boundaries=bwboundaries(Ibw);

%%
% 连通域的填充
for k=1:m
    b=boundaries{k};
    switch k
        case 1
            fill(b(:,2),b(:,1),'w');
        case 2
            fill(b(:,2),b(:,1),'g');
        case 3
            fill(b(:,2),b(:,1),'b');
        case 4
            fill(b(:,2),b(:,1),[1 0.5 0.5]);
    end
end

%%
% 与运算,去除背景，保留工件
[M,N] = size(Ibw);
for i=1:M
    for j=1:N
        if Ibw(i,j)==0
            I1(i,j) = 0;
        end
    end
end
figure;
imshow(I1); 

%%
% sobel边沿检测，包含背景
ecanny_bwI1 = edge(I1,'sobel');
figure;
imshow(ecanny_bwI1); 
title('sobel edge');

%%
% 后处理、形态学运算
% % strel_mat = strel('square',3);
% % close_I1 = imclose(ecanny_bwI1,strel_mat);

% % figure;
% % imshow(close_I1); 
% % title('close bwImages');

%%
% 再次提取边界
figure;
imshow(ecanny_bwI1); 
title('bw Images');
hold on;

[L,m]=bwlabel(ecanny_bwI1,4);
boundaries=bwboundaries(ecanny_bwI1);

%%
% 边界去除,最长的被认为是边界
[M,N] = size(boundaries{3});
temp = boundaries{3};
for count=1:M
     i = temp(count,1);
     j = temp(count,2);
     ecanny_bwI1(i,j) = 0;
end
figure;
imshow(ecanny_bwI1); 
title('去除边界之后的边沿，包含焊缝');

%%
% 在原图中可视化边界
I1  = imread('workspiece.bmp');% 导入图像
I1  = rgb2gray(I1);
[M,N] = size(ecanny_bwI1);
for i=1:M
    for j=1:N
        if ecanny_bwI1(i,j)==1
            I1(i,j) = 255;
        end
    end
end
figure;
imshow(I1); 

%%
% 最长曲线的搜索，焊缝的提取
[L,m]=bwlabel(ecanny_bwI1,8);
[M,N] = size(ecanny_bwI1);
num = zeros(12,1);
for i=1:M
    for j=1:N
        switch L(i,j)
            case 1
               num(1) = num(1)+1;
            case 2
                num(2) = num(2)+1;
            case 3
                num(3) = num(3)+1;
            case 4  
                num(4) = num(4)+1;
            case 5
                num(5) = num(5)+1;
            case 6
                num(6) = num(6)+1;
            case 7
                num(7) = num(7)+1;
            case 8
                num(8) = num(8)+1;
            case 9 
                num(9) = num(9)+1;
            case 10
                num(10) = num(10)+1;
            case 11
                num(11) = num(11)+1;
            case 12
                num(12) = num(12)+1;
            otherwise
              
        end         
    end
end

%%
% 在原图中可视化焊缝
I1  = imread('workspiece.bmp');% 导入图像
I1  = rgb2gray(I1);
[M,N] = size(ecanny_bwI1);
for i=1:M
    for j=1:N
        if L(i,j)==7
            I1(i,j) = 255;
        end
    end
end
figure;
imshow(I1); 









