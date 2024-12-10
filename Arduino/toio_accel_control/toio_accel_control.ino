#include <M5Unified.h>
#include <Toio.h>

// toio コア キューブ（単体）付属の簡易プレイマット
const int MAT_CENTER_X = 98 + (402 - 98) / 2;
const int MAT_CENTER_Y = 142 + (358 - 142) / 2;

static bool mode = false;  // デモの状態

Toio toio;                     // Toio オブジェクト
ToioCore* toiocore = nullptr;  // ToioCore オブジェクトポインタ

void setup() {
  Serial.begin(115200);

  std::vector<ToioCore*> toiocore_list = toio.scan(3);  // 3 秒間 Toio Core Cube をスキャン

  // 見つからなければ終了
  size_t n = toiocore_list.size();
  if (n == 0) {
    exit(-1);
  }

  toiocore = toiocore_list.at(0);  // 最初に見つかった Toio Core Cube の ToioCore オブジェクトを取得

  toiocore->connect();  // toioに接続する

  // for M5Stack
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.init();
}

void loop() {
  float ax, ay, az;

  M5.update();
  toio.loop();

  // toioのマット上での位置と角度を表示
  ToioCoreIDData data = toiocore->getIDReaderData();
  if (data.type == ToioCoreIDTypePosition) {
    Serial.printf("posX: %u posY: %u angle: %u\n",
                  data.position.cubePosX, data.position.cubePosY, data.position.cubeAngleDegree);
  } else if (data.type == ToioCoreIDTypeStandard) {
    Serial.printf("Standard ID: %u\n", data.standard.standardID);
  }

  // Batteryの状態を表示
  uint8_t batt_level = toiocore->getBatteryLevel();
  Serial.println(String(batt_level) + "%");

  // 加速度の取得
  M5.Imu.getAccel(&ax, &ay, &az);  // get accel
  Serial.printf("ax: %f ay: %f az: %f\n", ax, ay, az);

  // toioの移動
  toiocore->controlMotorWithTarget(0, 5, 0, 50, 0, MAT_CENTER_X + 100 * -ax, MAT_CENTER_Y + 100 * ay, 0);


  // LEDの点滅
  if (mode) {
    toiocore->turnOffLed();  // 点灯中なら消灯
  } else {
    toiocore->turnOnLed(255, 255, 255);  // 消灯中なら白で点灯
  }
  mode = !mode;
}
