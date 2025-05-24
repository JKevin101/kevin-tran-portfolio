"""
Kevin Tran
ECE 4354
Final Project
4/25/2025

Usage:
   python Final_Project_Code.py {<video device number>|<video file name>}

Keyboard shortcuts: (video display window must be selected)
   ESC - exit
"""
import cv2
import mediapipe as mp
import serial
import time

# Connecting to Arduino
arduino = serial.Serial('COM5', 9600)
time.sleep(2)

# Initialize MediaPipe and OpenCV
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(max_num_hands=1, min_detection_confidence=0.75)
mp_draw = mp.solutions.drawing_utils

# Default webcam
cap = cv2.VideoCapture(0)
print("Hand tracking started. Press ESC to quit.")


# Determine if fingers are extended
def get_finger_states(landmarks):
    states = []

    # Thumb: x-axis (tip vs joint)
    thumb = landmarks[4].x < landmarks[3].x
    states.append(int(thumb))

    # Index to pinky: y-axis (tip higher than lower joint)
    for tip_id in [8, 12, 16, 20]:
        is_extended = landmarks[tip_id].y < landmarks[tip_id - 2].y
        states.append(int(is_extended))
    return states


while True:
    success, frame = cap.read()
    if not success:
        continue
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb)

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            mp_draw.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
            finger_states = get_finger_states(hand_landmarks.landmark)
            data = ','.join(map(str, finger_states)) + '\n'
            print(f"Sending: {data.strip()}")

            # Sending over serial
            arduino.write(data.encode())
            time.sleep(0.01)

    cv2.imshow("Hand Tracking", frame)

    # Exit on ESC key (ASCII 27)
    if cv2.waitKey(1) == 27:
        print("🛑 ESC pressed. Exiting...")
        break


cap.release()
arduino.close()
cv2.destroyAllWindows()