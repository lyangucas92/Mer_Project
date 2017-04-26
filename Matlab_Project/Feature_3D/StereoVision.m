%%
% ˫Ŀ�����Ӿ�
% �Ա�ʵ��

%%
% ��չ�����
clc;
clear;
close all;

%%
% ����ͼ������
I1 = imread('pattern_cam1_im1.png');
I2 = imread('pattern_cam2_im1.png');
figure
imshowpair(I1, I2, 'montage'); 
title('Original Images');
% �����������
load cameraParams.mat

%%
% У��
I1 = undistortImage(I1, camera1Params);
I2 = undistortImage(I2, camera1Params);
figure 
imshowpair(I1, I2, 'montage');
title('Undistorted Images');

%%
% ��������ȡ
imagePoints1 = detectMinEigenFeatures(rgb2gray(I1), 'MinQuality', 0.1);

%%
% ���ӻ�
figure
imshow(I1, 'InitialMagnification', 50);
title('150 Strongest Corners from the First Image');
hold on
plot(selectStrongest(imagePoints1, 150));

%%
% Create the point tracker
tracker = vision.PointTracker('MaxBidirectionalError', 1, 'NumPyramidLevels', 5);
imagePoints1 = imagePoints1.Location;
initialize(tracker, imagePoints1, I1);
% Track the points
[imagePoints2, validIdx] = step(tracker, I2);
matchedPoints1 = imagePoints1(validIdx, :);
matchedPoints2 = imagePoints2(validIdx, :);

%%
% ������ƥ��
figure
showMatchedFeatures(I1, I2, matchedPoints1, matchedPoints2);
title('Tracked Features');

%%
% F�������
[fMatrix, epipolarInliers] = estimateFundamentalMatrix(...
  matchedPoints1, matchedPoints2, 'Method', 'MSAC', 'NumTrials', 10000);
% ����
inlierPoints1 = matchedPoints1(epipolarInliers, :);
inlierPoints2 = matchedPoints2(epipolarInliers, :);
% ��ʾ�ڵ�
figure
showMatchedFeatures(I1, I2, inlierPoints1, inlierPoints2);
title('Epipolar Inliers');

%%
% R��T
R = [0.9455,-0.0096,0.3253;
    0.0120,0.9999,-0.0053;
    -0.3252,0.0090,0.9456];
t = [98.4069,0.1741,18.9018];

%%
% ���ܵ�������
imagePoints1 = detectMinEigenFeatures(rgb2gray(I1), 'MinQuality', 0.001);

%%
% Create the point tracker
tracker = vision.PointTracker('MaxBidirectionalError', 1, 'NumPyramidLevels', 5);
% Initialize the point tracker
imagePoints1 = imagePoints1.Location;
initialize(tracker, imagePoints1, I1);
% Track the points
[imagePoints2, validIdx] = step(tracker, I2);
matchedPoints1 = imagePoints1(validIdx, :);
matchedPoints2 = imagePoints2(validIdx, :);

%%
% cameraMatrix
camMatrix1 = cameraMatrix(camera1Params, eye(3), [0 0 0]);
camMatrix2 = cameraMatrix(camera2Params, R', -t*R');

% ��ά���Ƶļ���
points3D = triangulate(matchedPoints1, matchedPoints2, camMatrix1, camMatrix2);

% ��ȡ��ɫ��Ϣ
numPixels = size(I1, 1) * size(I1, 2);
allColors = reshape(I1, [numPixels, 3]);
colorIdx = sub2ind([size(I1, 1), size(I1, 2)], round(matchedPoints1(:,2)), ...
    round(matchedPoints1(:, 1)));
color = allColors(colorIdx, :);

% ��������
ptCloud = pointCloud(points3D, 'Color', color);

%%
% ���ӻ�
cameraSize = 10;
figure
plotCamera('Size', cameraSize, 'Color', 'r', 'Label', '1', 'Opacity', 0);
hold on
grid on
plotCamera('Location', t, 'Orientation', R, 'Size', cameraSize, ...
    'Color', 'b', 'Label', '2', 'Opacity', 0);

% ���ƵĿ��ӻ�
pcshow(ptCloud, 'VerticalAxis', 'y', 'VerticalAxisDir', 'down', ...
    'MarkerSize', 45);

% Rotate and zoom the plot
camorbit(0, -30);
camzoom(1.5);

% Label the axes
xlabel('x-axis');
ylabel('y-axis');
zlabel('z-axis')
title('Up to Scale Reconstruction of the Scene');


