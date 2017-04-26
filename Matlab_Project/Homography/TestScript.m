% TEST SCRIPT OF THE FUNCTION "CONTOUR_FOLLOWING"


%test images opening
I1=rgb2gray(imread('test1.bmp'));
I2=rgb2gray(imread('test2.bmp'));
I3=rgb2gray(imread('test3.bmp'));
I4=rgb2gray(imread('test4.bmp'));
I5=rgb2gray(imread('test5.bmp'));


%function use
C1 = contour_following(I1);
C2 = contour_following(I2);
C3 = contour_following(I3);
C4 = contour_following(I4);
C5 = contour_following(I5);


%plotting the found contours
figure, imshow(I1);
figure, plot(C1(:,1),C1(:,2),'*');
for j=1:size(C1,1),
    text(C1(j,1),C1(j,2),['  ' num2str(j)]);
end

figure, imshow(I2);
figure, plot(C2(:,1),C2(:,2),'*');
for j=1:size(C2,1),
    text(C2(j,1),C2(j,2),['  ' num2str(j)]);
end

figure, imshow(I3);
figure, plot(C3(:,1),C3(:,2),'*');
for j=1:size(C3,1),
    text(C3(j,1),C3(j,2),['  ' num2str(j)]);
end

figure, imshow(I4);
figure, plot(C4(:,1),C4(:,2),'*');
for j=1:size(C4,1),
    text(C4(j,1),C4(j,2),['  ' num2str(j)]);
end

figure, imshow(I5);
figure, plot(C5(:,1),C5(:,2),'*');
for j=1:size(C5,1),
    text(C5(j,1),C5(j,2),['  ' num2str(j)]);
end
