1. 📚 Phần khai báo (Ở đầu file)
/* USER CODE BEGIN Includes */

Chức năng: Nơi để bạn thêm các lệnh #include của riêng mình.

Ví dụ: #include "my_gpio_driver.h"

/* USER CODE BEGIN PTD */ (Private TypeDef)

Chức năng: Nơi để định nghĩa các typedef (struct, enum) của riêng bạn.

Ví dụ: typedef struct { ... } SensorData_t;

/* USER CODE BEGIN PD */ (Private Define)

Chức năng: Nơi để thêm các lệnh #define hằng số.

Ví dụ: #define LED_PIN 5

/* USER CODE BEGIN PV */ (Private Variables)

Chức năng: Nơi để khai báo các biến toàn cục (global variables).

Ví dụ: uint32_t g_tick_count = 0;

/* USER CODE BEGIN PFP */ (Private Function Prototypes)

Chức năng: Nơi khai báo "prototype" (nguyên mẫu hàm) cho các hàm bạn tự viết.

Ví dụ: void My_Clock_Init(void);

/* USER CODE BEGIN 0 */

Chức năng: Nơi để viết các hàm (ví dụ: hàm xử lý ngắt SysTick_Handler) trước hàm main().

2. ⚙️ Bên trong hàm main()
/* USER CODE BEGIN 1 */

Chức năng: Nơi để code chạy ngay khi vào main().

Vị trí VÀNG cho code bare-metal: Đây là nơi hoàn hảo để bạn đặt hàm khởi tạo clock (RCC) của mình.

/* USER CODE BEGIN Init */ & /* USER CODE BEGIN SysInit */

Chức năng: Dùng để khởi tạo các thành phần hệ thống khác. Bạn có thể đặt hàm khởi tạo Systick của mình ở đây.

/* USER CODE BEGIN 2 */

Chức năng: Nơi để code khởi tạo trước vòng lặp while(1).

Vị trí VÀNG cho code bare-metal: Đây là nơi hoàn hảo để bạn đặt hàm khởi tạo ngoại vi (GPIO, UART, Timers...) sau khi clock đã chạy ổn định.

/* USER CODE BEGIN 3 */

Chức năng: Nơi chứa code logic chính của bạn.

Vị trí VÀNG cho code bare-metal: Code của bạn trong vòng lặp while(1) sẽ được đặt ở đây.

3. 📝 Phần định nghĩa hàm (Ở cuối file)
/* USER CODE BEGIN 4 */

Chức năng: Nơi để viết nội dung/định nghĩa cho các hàm mà bạn đã khai báo ở PFP.

Ví dụ:

C

void My_Clock_Init(void)
{
  // Code RCC bare-metal của bạn
}