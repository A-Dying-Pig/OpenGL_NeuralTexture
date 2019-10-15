import cv2
import os

image_folder = 'vase_frames_2000'
video_name = 'video.mp4'
fps = 30

images = []
for i in range(0,2000):
    images.append(str(i).zfill(4)+'.png')

frame = cv2.imread(os.path.join(image_folder, images[0]))
height, width, layers = frame.shape

fourcc = cv2.VideoWriter_fourcc('D', 'I', 'V', 'X')
video = cv2.VideoWriter(video_name, fourcc, fps, (width,height))     #third parameter - fps

for image in images:
    video.write(cv2.imread(os.path.join(image_folder, image)))

cv2.destroyAllWindows()
video.release()