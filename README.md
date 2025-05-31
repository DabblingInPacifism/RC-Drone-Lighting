Hi there! This guide will attempt to get you through the process of adding addressable LED lighting to your RC projects. As i learn, the scope of this project will expand, but i will be initially focused on general information and lighting for 75mm racing whoops, specifically meeeting [UDL Igniter specs.](https://undergrounddroneleague.com/)

Many many many many thanks to Jan Jaap and his [FPV Remote Visualizer,](https://github.com/UAV-Painkillers/fpv-remote-visualizer) which is a modification to add customizable LED rings around the Jumper T20s gimbals, but it can be used as the basis for remotely controlling the LEDs via your own controller.

# Intro

## Knowledge Prerequisites/Who is this for?

  If you can build and program your own drone, you can probably get through this no problem. **You do not need to know how to code or anything about embedded systems to get sick ass custom lighting on your racing whoop.** Every bit of knowledge makes this process easier, but i hadn't coded with any seriousness since more or less highschool before doing this. The tools that are available today make a project like this possible without deep programming knowledge. I'm going to attempt to present this in a way that you can skip the technical section if you choose just to get the lighting up and running, but the technical section will be the most relevent bits of electronics, lighting, embeded systems and coding that got me from basically 0 foundational knowledge to messing with this stuff.

## Disclaimer!

  I'm not in software development or tech, am a tradesman without a formal education, am doing this stuff alongside maintaining professional certifications, education, my family and... quite honestly, i don't even regard myself as particularly bright, so keep all that in mind when reading this. If i got something wrong, please correct me and i'll update the repo to reflect! I'm just a guy trying to make some cool stuff.

# Light up your Whoop
## General overview

  We're going to be using an ATTiny85 microcontroller to drive two strips of WS2812B COB leds in a rotating rainbow pattern. We'll use ChatGPT to generate the code we need, program our chip and put everything together.

## What we'll need.

Parts:
- [2.7mm 5V COB LEDs WS2812B or similar](https://www.alibaba.com/product-detail/2-7mm-COB-LED-Strip-Ultra_1601423996981.html)
- [ATTiny85 MCU](https://www.alibaba.com/product-detail/ATTINY85-20PU-ATTINY85-Attiny85-Microcontroller-DIP8_11000018774516.html)
- [Tiny AVR Programmer](https://www.amazon.com/SparkFun-Tiny-AVR-Programmer/dp/B00B6KNJRY)
- [Gorilla Double Sided tape](https://gorillatough.com/product/gorilla-double-sided-tape/)
- Wire, soldering kit, heat shrink.

Software:
- [Arduino IDE](https://support.arduino.cc/hc/en-us/articles/360019833020-Download-and-install-Arduino-IDE)
- [Tiny AVR Programmer drivers](https://learn.sparkfun.com/tutorials/tiny-avr-programmer-hookup-guide/#automatic-install) (This page will also show you how to enable the ATTiny plugin in Arduino IDE.)
