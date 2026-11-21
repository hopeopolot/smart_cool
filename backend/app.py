
from pathlib import Path

import joblib
import pandas as pd

from flask import Flask, request, jsonify
from flask_cors import CORS


# ==================================================
# 1. FLASK APPLICATION SETUP
# ==================================================

app = Flask(__name__)
CORS(app)


# ==================================================
# 2. LOAD SMARTCOOL MACHINE LEARNING MODEL
# ==================================================

# app.py is inside Backend.
# parent.parent takes us back to the Capstone folder.

# Go from Backend to Capstone
BASE_DIR = Path(__file__).resolve().parent.parent

# Model is a sibling folder of Backend
MODEL_PATH = BASE_DIR / "Model" / "smartcool_model.pkl"

print("Project folder:", BASE_DIR)
print("Model path:", MODEL_PATH)
print("Model exists:", MODEL_PATH.exists())

if not MODEL_PATH.exists():
    raise FileNotFoundError(f"Model not found: {MODEL_PATH}")

model = joblib.load(MODEL_PATH)

print("SmartCool model loaded successfully!")

# ==================================================
# 3. SENSOR DATA STORAGE
# ==================================================

latest_readings = {
    "temperature": 0.0,
    "humidity": 0.0,
    "light": 0,
    "gas": 0,
    "fan": False,
    "gas_warning": False
}


# ==================================================
# 4. MACHINE LEARNING PREDICTION STORAGE
# ==================================================

latest_prediction = {
    "prediction": None,
    "message": "Waiting for sensor readings",
    "recommended_action": "Monitoring"
}


# Prototype raw ADC warning threshold.
# This is not a calibrated gas concentration.

GAS_WARNING_LEVEL = 2000


# ==================================================
# 5. RECEIVE SENSOR READINGS FROM ESP32
# ==================================================

@app.route("/api/readings", methods=["GET", "POST"])

def readings():

    global latest_readings

    # Receive data from ESP32
    if request.method == "POST":

        data = request.get_json(silent=True)

        if not data:
            return jsonify({
                "status": "error",
                "message": "No JSON data received"
            }), 400

        try:

            latest_readings["temperature"] = float(
                data.get("temperature", 0)
            )

            latest_readings["humidity"] = float(
                data.get("humidity", 0)
            )

            latest_readings["light"] = int(
                data.get("light", 0)
            )

            latest_readings["gas"] = int(
                data.get("gas", 0)
            )

            latest_readings["fan"] = bool(
                data.get("fan", False)
            )

            latest_readings["gas_warning"] = (
                latest_readings["gas"] >= GAS_WARNING_LEVEL
            )

        except (ValueError, TypeError):

            return jsonify({
                "status": "error",
                "message": "Invalid sensor data"
            }), 400

        print("\nNew ESP32 sensor readings received:")
        print(latest_readings)

        return jsonify({
            "status": "success",
            "message": "Sensor readings received successfully",
            "readings": latest_readings
        }), 200

    # Return latest readings to dashboard
    return jsonify(latest_readings)


# ==================================================
# 6. MACHINE LEARNING PREDICTION
# ==================================================

@app.route("/api/predict", methods=["POST"])

def predict():

    global latest_prediction

    data = request.get_json(silent=True)

    if not data:
        return jsonify({
            "error": "No prediction data received"
        }), 400

    try:

        temperature = float(data.get("temperature", 0))
        humidity = float(data.get("humidity", 0))

        # The trained model expects six input features:
        #
        # Temperature Front
        # Humidity Front
        # Temperature Middle
        # Humidity Middle
        # Temperature Back
        # Humidity Back
        #
        # Currently, SmartCool uses one SHT31 sensor.
        # Its readings are repeated to match the model inputs.
        # This is an approximation, not three independent measurements.

        input_data = pd.DataFrame(
            [[
                temperature,
                humidity,
                temperature,
                humidity,
                temperature,
                humidity
            ]],
            columns=[
                "Temperature Front",
                "Humidity Front",
                "Temperature Middle",
                "Humidity Middle",
                "Temperature Back",
                "Humidity Back"
            ]
        )

        # Run trained model
        prediction = int(model.predict(input_data)[0])

        if prediction == 1:

            message = "High temperature risk predicted"

            action = "Prepare for ventilation"

        else:

            message = "Temperature conditions are currently stable"

            action = "Continue monitoring"

        latest_prediction = {
            "prediction": prediction,
            "message": message,
            "recommended_action": action
        }

        print("\nSmartCool ML Prediction:")
        print(latest_prediction)

        return jsonify(latest_prediction), 200

    except (ValueError, TypeError) as error:

        return jsonify({
            "error": str(error)
        }), 400

    except Exception as error:

        print("Prediction error:", error)

        return jsonify({
            "error": "Unable to generate prediction"
        }), 500


# ==================================================
# 7. GET LATEST MACHINE LEARNING PREDICTION
# ==================================================

@app.route("/api/prediction", methods=["GET"])

def get_prediction():

    return jsonify(latest_prediction)


# ==================================================
# 8. HOME ROUTE
# ==================================================

@app.route("/", methods=["GET"])

def home():

    return jsonify({
        "project": "SmartCool",
        "status": "Backend is running",
        "message": "Greenhouse monitoring system API"
    })


# ==================================================
# 9. START SMARTCOOL SERVER
# ==================================================

if __name__ == "__main__":

    print("\n====================================")
    print("SmartCool server is starting...")
    print("Local address: http://127.0.0.1:5000")
    print("====================================\n")

    app.run(
        host="0.0.0.0",
        port=5000,
        debug=False
    )
