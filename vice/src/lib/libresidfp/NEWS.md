1.2.0 2026-07-xx
* Make DAC leakage adjustable (#10)
* Make the 6581 waveform offset adjustable (#13)



1.1.2 2026-07-17
* Removed typo in configure script



1.1.1 2026-06-28
* Properly store/restore paddle_y
* Reworked voice scaling to fix the DC offset on 6581



1.1.0 2026-06-21
* Reduced the 6581 filter curve parameter range to avoid overflows (#19)
* Clear filter state on model change (#27)
* Enabled silent make rules
* Added a new clockDigital function that only clocks the digital components (#25)
* Allow changing paddle coordinates (#28)
* Added peek function (#28)
* Allow save/restore state (#20)



1.0.3 2026-05-15
* Link with pthread (#15)
* Fixed a bunch of shadow warnings (#16)
* Restore pre-c++17 compatibility (#16)


1.0.1 2026-05-01
* Minor cleanups


1.0.0 2026-04-06
* Allow bypassing the resampler for raw output (#6)
* Add an overloaded clock function that fills the buffer (#7)


0.9.3 2026-03-22
* Make 6581 caps configurable (#4)


0.9.2 2026-03-01
* Rework the 6581 filter curve setting


0.9.1 2026-02-08
* Fixed typo in pkg config file
* Simplified locking for singleton initialization


0.9.0 2026-01-28
* First experimental release
