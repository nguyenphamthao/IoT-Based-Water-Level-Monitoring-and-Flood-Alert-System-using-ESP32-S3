from flask import Flask, request, jsonify, send_from_directory
import os

app = Flask(__name__)

# Biến lưu trữ dữ liệu mặc định ban đầu
latest_data = {
    "distance": 0,
    "water_level": 0,
    "is_raining": False,
    "rain_status": "Đang chờ dữ liệu...",
    "status": "Đang chờ kết nối..."
}


# Route 1: Cung cấp giao diện web khi truy cập vào localhost:3000
@app.route('/')
def index():
    return send_from_directory('public', 'index.html')


# Route 2: Xử lý dữ liệu
@app.route('/api/data', methods=['GET', 'POST'])
def handle_data():
    global latest_data

    if request.method == 'POST':
        # Nhận dữ liệu từ ESP32
        latest_data = request.json
        print(f"\n[+] Đã nhận dữ liệu mới từ ESP32: {latest_data}")
        return "Server đã nhận thành công", 200

    elif request.method == 'GET':
        # Gửi dữ liệu cho giao diện web cập nhật
        return jsonify(latest_data)


if __name__ == '__main__':
    print("==================================================")
    print("🚀 Web Server đang chạy tại: http://localhost:3000")
    print("🌐 Dùng lệnh sau ở Terminal khác để mở ngrok:")
    print("   ngrok http 3000")
    print("==================================================")
    app.run(port=3000, host='0.0.0.0')