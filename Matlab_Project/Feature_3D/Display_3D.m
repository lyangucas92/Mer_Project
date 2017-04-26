%%
% Clear
clc;
clear;
close all;

%%
% load txt data
ex = importdata('../pointcloud.txt');
ex=ex(1:30:end,:);

Point_X = ex(:,1); 
Point_Y = ex(:,2); 
Point_Z = ex(:,3); 

%% 
% 3d Display
scatter3(Point_X,Point_Y,Point_Z,'filled')
