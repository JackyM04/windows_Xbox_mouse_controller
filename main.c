#include <windows.h>
#include <Xinput.h>
#include <stdio.h>

int main() {
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    // 获取屏幕分辨率
    int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
    int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);

    // 手柄摇杆的最大值（半径）
    const SHORT MAX_THUMB = 32768;

    // 定义灵敏度（可以调整）
    const double SENSITIVITY = 0.3;
    const double SENSITIVITY_ST = 1.2;

    // 计算缩放因子
    const double SCALE_X = SENSITIVITY * (double)SCREEN_WIDTH / (2 * MAX_THUMB);
    const double SCALE_Y = SENSITIVITY * (double)SCREEN_HEIGHT / (2 * MAX_THUMB);
    const double SCALE_X_ST = SENSITIVITY_ST * (double)SCREEN_WIDTH / (2 * MAX_THUMB);
    const double SCALE_Y_ST = SENSITIVITY_ST * (double)SCREEN_HEIGHT / (2 * MAX_THUMB);

    // 定义扳机阈值
    const BYTE TRIGGER_THRESHOLD = 30;

    // 记录扳机的上一次状态，防止重复触发点击
    int leftTriggerPressed = 0;
    int rightTriggerPressed = 0;

    while (1) {
        // 获取手柄状态
        DWORD dwResult = XInputGetState(0, &state);

        if (dwResult == ERROR_SUCCESS) {
            // 手柄已连接
            SHORT thumbX = -state.Gamepad.sThumbLX; // 左摇杆X轴
            SHORT thumbY = -state.Gamepad.sThumbLY; // 左摇杆Y轴
            SHORT stX = -state.Gamepad.sThumbRX;    // 右摇杆X轴
            SHORT stY = state.Gamepad.sThumbRY;     // 右摇杆Y轴

            // 应用映射公式，将摇杆输入映射到屏幕坐标
            int mouseX = (int)(thumbX * SCALE_X + SCREEN_WIDTH / 2);
            int mouseY = (int)(SCREEN_HEIGHT / 2 - thumbY * SCALE_Y);

            mouseX = mouseX + (int)(stX * SCALE_X_ST);
            mouseY = mouseY + (int)(stY * SCALE_Y_ST);

            // 确保鼠标坐标在屏幕范围内
            if (mouseX < 0) mouseX = 0;
            if (mouseX > SCREEN_WIDTH - 1) mouseX = SCREEN_WIDTH - 1;
            if (mouseY < 0) mouseY = 0;
            if (mouseY > SCREEN_HEIGHT - 1) mouseY = SCREEN_HEIGHT - 1;

            // 设置新的鼠标位置
            SetCursorPos(mouseX, mouseY);

            // 获取扳机的值
            BYTE leftTrigger = state.Gamepad.bLeftTrigger;
            BYTE rightTrigger = state.Gamepad.bRightTrigger;

            // 处理左扳机（鼠标左键）
            if (leftTrigger > TRIGGER_THRESHOLD) {
                if (!leftTriggerPressed) {
                    // 扳机刚被按下，模拟鼠标左键按下
                    mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
                    leftTriggerPressed = 1;
                }
            } else {
                if (leftTriggerPressed) {
                    // 扳机已释放，模拟鼠标左键释放
                    mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
                    leftTriggerPressed = 0;
                }
            }

            // 处理右扳机（鼠标右键）
            if (rightTrigger > TRIGGER_THRESHOLD) {
                if (!rightTriggerPressed) {
                    // 扳机刚被按下，模拟鼠标右键按下
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    rightTriggerPressed = 1;
                }
            } else {
                if (rightTriggerPressed) {
                    // 扳机已释放，模拟鼠标右键释放
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    rightTriggerPressed = 0;
                }
            }

            // 输出调试信息
            printf("ThumbX: %d, ThumbY: %d, MouseX: %d, MouseY: %d\n", thumbX, thumbY, mouseX, mouseY);
        } else {
            // 手柄未连接
            printf("Controller not connected\n");
        }

        Sleep(10); // 防止占用过多CPU
    }

    return 0;
}
