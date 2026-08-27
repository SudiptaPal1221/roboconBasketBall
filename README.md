🏀 Basketball Rim Detection & Distance Estimation This project performs real-time basketball rim detection and distance estimation using a custom-trained YOLOv5 object detection model and OpenCV. The application processes video input, detects the basketball rim, and overlays both bounding boxes and the estimated distance from the camera.

🚀 Features ✅ Real-time basketball rim detection using YOLOv5

✅ Distance estimation using calibrated object size (rim diameter)

✅ OpenCV-based video processing and annotation

✅ Option to save output as annotated video

✅ Custom-trained model for high-accuracy detection

🧠 Model Training The YOLOv5 model was trained from scratch using a labeled dataset of basketball rims.

Training was done using PyTorch on GPU (cuda:0) with custom augmentation and tuned hyperparameters.

After sufficient training and validation, the best-performing model weights (best.pt) were used for inference.

📐 Distance Estimation Formula The real-world distance to the rim is estimated using the formula:

Distance
Real Rim Height (m) × Scaling Factor Pixel Height of Rim Distance= Pixel Height of Rim Real Rim Height (m)×Scaling Factor​

The scaling factor is computed during an initial calibration step, where the rim is captured from a known distance.

🖥️ Demo Example: input video vs. processed output with detection and distance



🧪 How to Run bash Copy Edit git clone https://github.com/SudiptaPal1221/roboconBasketBall.git cd basketball-rim-detection
Install requirements
pip install -r requirements.txt

Run detection
python rim_detector.py ⚙️ Configuration WEIGHTS_PATH: Path to your trained YOLOv5 weights

VIDEO_PATH: Input video for detection

OUTPUT_VIDEO_PATH: Annotated output video path

REAL_RIM_HEIGHT: Real-world rim diameter (typically 0.457m)

CALIBRATION_DISTANCE: Known distance (in meters) from which calibration is done

🛠 Tech Stack Python

OpenCV

PyTorch

YOLOv5

CUDA (for GPU acceleration)

📸 Example Output Frame 
