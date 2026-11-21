
const API_URL = "http://127.0.0.1:5000";

async function updateDashboard() {
    try {
        // Get the latest greenhouse readings
        const readingsResponse = await fetch(`${API_URL}/api/readings`);

        if (!readingsResponse.ok) {
            throw new Error("Could not retrieve sensor readings");
        }

        const data = await readingsResponse.json();

        // Display temperature
        document.getElementById("temperature").textContent =
            `${data.temperature} °C`;

        // Display humidity
        document.getElementById("humidity").textContent =
            `${data.humidity} %`;

        // Display light intensity
        document.getElementById("light").textContent = data.light;

        // Display fan status
        document.getElementById("fan").textContent =
            data.fan ? "ON" : "OFF";

        document.getElementById("predictionFan").textContent =
            data.fan ? "ON" : "OFF";

        // --------------------------------
        // GAS SENSOR DISPLAY
        // --------------------------------

        const gasValue = Number(data.gas) || 0;

        const gasElement = document.getElementById("gas");
        const gasBar = document.getElementById("gasBar");
        const gasStatus = document.getElementById("gasStatus");
        const gasIndicator = document.getElementById("gasIndicator");
        const gasMessage = document.getElementById("gasMessage");
        const gasCard = document.querySelector(".reading-card.gas");

        if (gasElement) {
            gasElement.textContent = gasValue;
        }

        if (gasBar) {
            const percentage = Math.min(
                (gasValue / 4095) * 100,
                100
            );

            gasBar.style.width = `${percentage}%`;
        }

        if (gasValue >= 2000) {
            if (gasStatus) {
                gasStatus.textContent = "High reading";
                gasStatus.classList.add("gas-status-warning");
            }

            if (gasBar) gasBar.classList.add("warning");

            if (gasIndicator) {
                gasIndicator.style.color = "#d96850";
            }

            if (gasMessage) {
                gasMessage.textContent =
                    "Reading is above the warning threshold. Check greenhouse air conditions.";
            }

            if (gasCard) gasCard.classList.add("high-gas");

        } else {
            if (gasStatus) {
                gasStatus.textContent = "Within warning limit";
                gasStatus.classList.remove("gas-status-warning");
            }

            if (gasBar) gasBar.classList.remove("warning");

            if (gasIndicator) {
                gasIndicator.style.color = "#55a66c";
            }

            if (gasMessage) {
                gasMessage.textContent =
                    "Reading is below the warning threshold.";
            }

            if (gasCard) gasCard.classList.remove("high-gas");
        }

        // --------------------------------
        // MACHINE LEARNING PREDICTION
        // --------------------------------

        const predictionResponse = await fetch(`${API_URL}/api/predict`, {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                temperature: data.temperature,
                humidity: data.humidity
            })
        });

        if (!predictionResponse.ok) {
            throw new Error("Could not get ML prediction");
        }

        const prediction = await predictionResponse.json();

        const isRisk = prediction.prediction_value === 1;

        document.getElementById("predictionStatus").textContent =
            isRisk ? "OVERHEATING RISK" : "NORMAL";

        document.getElementById("predictionMessage").textContent =
            prediction.alert;

        // --------------------------------
        // SYSTEM ALERTS
        // --------------------------------

        const gasWarning = gasValue >= 2000;

        document.getElementById("alertTitle").textContent =
            gasWarning
                ? "Gas Sensor Warning"
                : isRisk
                    ? "Overheating Warning"
                    : "System Operating";

        document.getElementById("alertMessage").textContent =
            gasWarning
                ? "Gas sensor reading is above the configured warning threshold."
                : prediction.alert;

        document.getElementById("alertSymbol").textContent =
            gasWarning || isRisk ? "!" : "✓";

    } catch (error) {
        console.error("Dashboard error:", error);
    }
}

// Update immediately and then every 2 seconds
updateDashboard();
setInterval(updateDashboard, 2000);
