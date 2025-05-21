# Game Rắn Săn Mồi (SDL Snake Game)

## Thông Tin Sinh Viên

* **Họ và tên:** Nguyễn Quốc Sỹ
* **MSSV:** 24021617
* **Chủ đề game:** Rắn Săn Mồi (Snake Game)
* **Ngưỡng điểm mong muốn:** 7-8/10

## Mô Tả Game

Đây là một phiên bản của trò chơi Rắn Săn Mồi cổ điển, được phát triển bằng ngôn ngữ C++ và thư viện SDL2. Người chơi sẽ điều khiển một con rắn di chuyển trên màn hình, cố gắng ăn các loại mồi để tăng điểm và độ dài, đồng thời tránh đâm vào tường (nếu có) hoặc tự đâm vào thân mình. Game có các loại mồi khác nhau, bao gồm cả mồi giúp rắn tăng tốc tạm thời.

## Các Tính Năng Chính (Đã thực hiện / Hướng tới)

### Logic Chính Của Game:
* **Điều khiển rắn:** Người chơi điều khiển rắn di chuyển theo bốn hướng (LÊN, XUỐNG, TRÁI, PHẢI) bằng các phím mũi tên trên bàn phím. Rắn không thể tự quay đầu 180 độ ngay lập tức.
* **Mồi (Food):**
    * Mồi thường xuất hiện ngẫu nhiên trên màn hình.
    * Khi rắn ăn mồi thường, rắn sẽ dài ra và người chơi được cộng điểm.
    * Có loại mồi đặc biệt (ví dụ: "quả bóng đỏ") giúp rắn tăng tốc độ di chuyển trong một khoảng thời gian nhất định (ví dụ: 3 giây).
* **Va chạm:**
    * Xử lý va chạm khi đầu rắn ăn mồi.
    * Xử lý va chạm khi đầu rắn tự đâm vào thân mình (kết thúc game).
    * Xử lý va chạm khi rắn đi ra khỏi biên màn hình (rắn xuất hiện ở cạnh đối diện).
* **Hiển thị rắn:**
    * Đầu rắn, thân rắn và đuôi rắn được vẽ bằng các hình ảnh riêng biệt.
    * Các đốt thân cong được sử dụng để tạo hiệu ứng rắn di chuyển và uốn lượn mượt mà hơn (sử dụng các ảnh như `curve.png`, `curve_tail.png`).

### Quản Lý Vòng Lặp Game:
* **Nền game:** Có thể sử dụng màu nền hoặc hình ảnh nền.
* **Tạm dừng và Tiếp tục:** Người chơi có thể tạm dừng game bằng nút "Pause" trực quan trên màn hình (hoặc phím 'P') và tiếp tục chơi.
* **Menu Tạm Dừng:** Khi game tạm dừng, một menu sẽ hiển thị với các tùy chọn "Resume" (Tiếp tục), "Replay" (Chơi lại), "Options" (Tùy chỉnh).

### Hệ Thống Điểm:
* **Cộng điểm:** Điểm được cộng khi rắn ăn mồi (mồi đặc biệt có thể cho nhiều điểm hơn).
* **Hiển thị điểm:** Điểm số hiện tại của người chơi được hiển thị liên tục trong quá trình chơi.
* **Lưu điểm cao nhất:** Khi trò chơi kết thúc, điểm số cao nhất đạt được sẽ được lưu lại (sử dụng `HighScoreManager`).

### Giao Diện Người Dùng (UI):
* **Menu Chính:** Giao diện menu chính với nút "Play" (thông qua phím Enter) và nút bật/tắt âm thanh trực quan.
* **Menu Tạm Dừng:** Hiển thị khi game tạm dừng, bao gồm các nút "Resume", "Replay", "Options".
* **Menu Options:** Cho phép người chơi tùy chỉnh âm lượng nhạc nền và hiệu ứng âm thanh.
* **Màn hình Game Over:** Hiển thị điểm số cuối cùng, điểm cao nhất, và nút "CHƠI LẠI" trực quan (thay cho việc chỉ nhấn phím 'R').

### Âm Thanh:
* **Nhạc nền và Hiệu ứng:** Sử dụng thư viện SDL_mixer để phát nhạc nền và các hiệu ứng âm thanh (ăn mồi, thua game, click chuột).
* **Bật/Tắt Âm Thanh:** Có nút trực quan ở Menu chính để bật/tắt toàn bộ âm thanh.
* **Tùy chỉnh âm lượng:** Cho phép điều chỉnh âm lượng nhạc nền và hiệu ứng âm thanh trong Menu Options.

### Tài Nguyên (Assets):
* **Hình ảnh:** Load và hiển thị các hình ảnh cho đầu rắn (`snake.png`), thân thẳng (`body.png`), đuôi cuối (`last_tail.png`), các loại khúc cua (`curve.png`, `curve_tail.png`), các loại mồi, hình nền (nếu có), và các nút bấm giao diện (nút Pause, nút Mute/Unmute).
* **Font chữ:** Sử dụng file font `.ttf` và thư viện SDL_ttf để hiển thị điểm số và các văn bản trên menu, nút bấm.

## Các Phần Có Tham Khảo (Ví dụ)

* **Xử lý đồ họa:** Kỹ thuật render hình ảnh, xử lý spritesheet cho các đốt cong của rắn sử dụng thư viện SDL2.
* **Quản lý tài nguyên:** Sử dụng các class `TextureManager` và `AudioManager` để load và quản lý tập trung các file ảnh và âm thanh.
* **Hiển thị văn bản:** Sử dụng `TTF_RenderText_Solid` và các hàm liên quan của SDL_ttf.

## Nguồn Tài Liệu Tham Khảo (Ví dụ)

* **Code mẫu và tutorial:** Từ tài liệu bổ sung và các bài giảng của môn Lập Trình Nâng Cao (LTNC), các ví dụ về SDL trên mạng.
* **Lazy Foo' Productions SDL Tutorials:** (Nếu bạn có tham khảo), các nguồn của AI
* **Hình ảnh và âm thanh:** Thu thập từ các nguồn tài nguyên miễn phí hoặc có giấy phép phù hợp trên mạng Internet (ví dụ: OpenGameArt, itch.io, v.v.).

## Hướng Phát Triển Tương Lai (Ví dụ)

* Thêm các loại mồi khác nhau với hiệu ứng đặc biệt (ví dụ: mồi làm chậm rắn, mồi trừ điểm, mồi xuyên tường).
* Thêm chướng ngại vật trên màn hình.
* Cải tiến AI cho rắn (nếu có chế độ máy chơi).
* Lưu trạng thái game để có thể tiếp tục chơi sau.
* Thêm các hiệu ứng hình ảnh (ví dụ: hiệu ứng khi ăn mồi, khi rắn chết).

## Hướng Dẫn Cài Đặt và Chạy Game (Ví dụ)

1.  **Yêu cầu thư viện:**
    * SDL2 (core)
    * SDL2_image
    * SDL2_ttf
    * SDL2_mixer
2.  **Cấu trúc thư mục (khuyến nghị):**
    * Thư mục gốc project
        * `main.cpp`, `Game.cpp`, `Game.h`, `Snake.cpp`, `Snake.h`, `Tails.cpp`, `Tails.hpp`, `Food.cpp`, `Food.h`, `TextureManager.cpp`, `TextureManager.h`, `AudioManager.cpp`, `AudioManager.h`, `HighScoreManager.cpp`, `HighScoreManager.h`, `Constants.h`, `Vector2D.hpp`, `Utils.cpp` (nếu có), `Utils.hpp` (nếu có).
        * Thư mục `assets/` (hoặc tên khác, hoặc để ngang cấp file exe): chứa tất cả các file ảnh (`.png`) và file âm thanh (`.wav`, `.mp3`), file font (`.ttf`).
3.  **Biên dịch (ví dụ với MinGW trên Windows và Code::Blocks):**
    * Tạo project C++ trong Code::Blocks.
    * Thêm tất cả các file `.cpp` vào project.
    * Cấu hình Linker settings để liên kết với các thư viện SDL2, SDL2_image, SDL2_ttf, SDL2_mixer (ví dụ: `-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer`).
    * Đảm bảo các file DLL của SDL nằm trong thư mục chứa file thực thi hoặc trong PATH hệ thống.
4.  **Chạy game:**
    * Thực thi file `.exe` đã được biên dịch.

---
