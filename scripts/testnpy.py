import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import os
import cv2


output_directory = "./"
filename = "0000.npy"

result_dir = "result"
ret = np.load(os.path.join(output_directory, filename))
print(ret.shape)
height = ret.shape[0]
width = ret.shape[1]


# for i in range(0,height):
#     _ = plt.hist(ret[i], bins=[-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0])
#     plt.savefig(os.path.join("result",str(i) + '.png'))
#     plt.clf()


# video_name = 'video.mp4'
# fps = 15
#
# images = []
# for i in range(0,2000):
#     images.append(str(i)+'.png')
#
# frame = cv2.imread(os.path.join(result_dir, images[0]))
# height, width, layers = frame.shape
#
# fourcc = cv2.VideoWriter_fourcc('D', 'I', 'V', 'X')
# video = cv2.VideoWriter(video_name, fourcc, fps, (width,height))     #third parameter - fps
#
# for image in images:
#     video.write(cv2.imread(os.path.join(result_dir, image)))
#
# cv2.destroyAllWindows()
# video.release()

