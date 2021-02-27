# CS120BLab11

Exercise 1:
Design a system where a ‘char’ variable can be incremented or decremented based on specific button presses. The value of the variable is then transmitted to the shift register and displayed on a bank of eight LEDs.

Criteria:

Two buttons are required. One button increments the variable. The second button decrements the variable.

The variable cannot be incremented past 0xFF or decremented below 0x00.

If both buttons are pressed simultaneously, the variable is reset to 0x00.

The variable is transmitted, and displayed, on the shift register whenever the variable changes value.

Demo: https://youtu.be/8SarsukUeCY

Exercise 2:
Design a system where one of three festive light displays is displayed on the shift register’s LED bank. The choice of festive light displays is determined by button presses.

Criteria:

Two buttons are used to cycle through the three available festive light displays.

One button cycles up, the other button cycles down.

Pressing both buttons together toggles the system on or off

Choose a default display for the system to start with. Whenever the system is turned on, the display shown on the LEDs should start with the default display.

Demo: https://youtu.be/m56DXICSF5s

Exercise 3:
Expand upon part 2 of the lab by adding another shift register and two buttons. Design a system where two different festive light displays can be displayed on two separate shift registers simultaneously. The selected festive light display for each shift register, is determined by the two buttons designated to that shift register. You can use the LED Bar if you don’t have enough discrete LEDs.

Criteria:

Two buttons control the festive light display shown on shift register 1. The other two buttons control the festive light display shown on shift register 2.

For each shift register, if their designated buttons are pressed simultaneously, the LED display on that shift register is toggled on or off.

The two shift registers should share the following lines: SER and SRCLK.

RCLK and SRCLR should be independent for each shift register.

When writing to either shift register, the same “transmit_data” function should be called.

Demo: https://youtu.be/kbiRvTRLJKU 

Exercise 4:
Expand upon part 1 of the lab by “Daisy Chaining” two shift registers (Sreg1 and Sreg2) together and display a ‘short’ variable across these two shift registers, instead of a ‘char’ variable on a single shift register.

Demo: https://youtu.be/9T8oGKu20EM

Exercise 5:
Using two “Daisy Chained” shift registers, design the following game. A player controller illuminated LED needs to avoid a collision with a game controlled (enemy) illuminated LED. The player can move their LED left or right, and can jump left or right. A collision can be avoided by jumping over the enemy LED.

Demo: https://youtu.be/8fb_zpkp7zA
