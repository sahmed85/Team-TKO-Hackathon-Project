# Team-TKO-Hackathon-Project
Team Training and Karate Optimized  
“Get hits to get fit” 
 

TKO Team: 
Shadman Ahmed, Mohammed Ahsan, Charles (Trey) Scarborough  

Fall 2020 Flowers Invention Studio Hackathon 

Disclaimer: TKO is by no means in any way affiliated with Black Desert Online, Pearl Abyss, Kakao and its affiliates.  


Project Outline: 
The product at its core is an interactive peripheral for video game control to encourage physicality without effecting immersion. At its current configuration the code is set up for the control of the Musa Class in the MMORPG Black Dessert Online. The main idea was to get an interactive way to exercise but also make it enjoyable, the workout should feel as if it is a byproduct, rather than the main goal. In addition, this system is much cheaper than VR, while providing instant tactile feedback, improving the workout.  It features a punching bag wired with analog switches that register hits as key presses and then converts that into combinations of moves for the player character. In addition, wrist mounted accelerometers control camera direction and player movement.  

Design Process: 
The initial idea was to use hard buttons for the punching bag, however the team faced significant difficulties with regarding the mounting and rigidity of the sensors onto the bag. In addition, it was difficult to engage the switch without the use of excessive force, making it very hard to chain combination of hits to the bag as the sensor would often to fail to read the input.  A further problem surfaced with repeated testing, as the buttons were prone to breaking. The combination of these elements led the team to pursue a different methodology for switches. After considerable research into different sensing methods, Velostat was selected as sensor of choice, for its cheap price, availability, and flexible form factor. This solved a lot of the problems with the initial concept, however Velostat can be very sensitive, and would often detect ghost inputs caused by motion of the punching bag. This led the development of a debouncing code to ensure that only intentional inputs were read. We then used the Arduino Keyboard library to send keyboard inputs to the game via the USB serial connection.  

After the hitting and combo mechanics of the system were ironed out the next key task was the movement of the player character on screen. The idea was to use a motion sensor to find the position of the user with respect to the punching bag, however this system lacked the precision, and tended to accumulate error over time. The team eventually decided to move from this idea and use the simpler, though more mathematically intensive, idea of using accelerometers. The accelerometer receives an input of the component of gravity acting on the sensor, this value is then compared to the relative angle the sensor makes with the floor (based on how much the user’s wrist is turned) and returns an angle of rotation for the player camera. In addition, two force sensors are mounted near each thumb that control camera rotation and character movement on the screen, as the camera angles in Black Desert Online are not bound to the player.  

Limitations:   
The anticheat mechanics of the game prevented the use of MATLAB and/or python code, an Arduino was elected as it is considered as a peripheral device. Due to time constraints and hardware issue, the team couldn’t get the Bluetooth module working in time to meet deadlines. Additionally, the motion control code is generalized for any game, however for this hackathon greater emphasis has been placed on the Musa class from Black Desert Online.  

List of Hardware and Software Utilized: 

Hardware: 

  Arduino Leonardo (1) 

  Arduino Micro (1) 

  Homemade Force Sensors using Velostat (10) 

  MPU 6050 Accelorometor/Gyroscope (2) 

Software: 
  
  Arduino IDE  

  Arduino Keyboard and Mouse Library 
