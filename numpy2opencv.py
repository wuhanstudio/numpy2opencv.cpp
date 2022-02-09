import cv2
import numpy as np

noise = np.load('noise.npy')

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

if not cap.isOpened():
    print("Cannot open camera")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("Can't receive frame (stream end?). Exiting ...")
        break

    height, width, channel = frame.shape
    frame = cv2.resize(frame, (noise.shape[0], noise.shape[1]), cv2.INTER_AREA)

    frame = frame + (noise * 255.0).astype(np.int8)
    frame[frame < 0] = 0
    frame[frame > 255] = 255

    frame = cv2.resize(frame, (width, height), cv2.INTER_AREA)

    cv2.imshow('frame', frame.astype(np.uint8))
    if cv2.waitKey(1) == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
