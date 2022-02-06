import cv2
import numpy as np

noise = np.load('noise.npy')

cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Cannot open camera")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("Can't receive frame (stream end?). Exiting ...")
        break

    frame = cv2.resize(frame, (416, 416), cv2.INTER_AREA)
    frame = frame + (noise * 255.0).astype(np.int8)
    frame[frame < 0] = 0
    frame[frame > 255] = 255

    cv2.imshow('frame', frame.astype(np.uint8))
    if cv2.waitKey(1) == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
