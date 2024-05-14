#pragma once

/// @brief Time for the generator to warm up in milliseconds.
const int DefaultGeneratorWarmUpTime = 1000 * 15;
const int DefaultGeneratorCoolDownTime = 1000 * 60;
/// @brief How much time to wait between stop checks.
const int DefaultGeneratorTimeToWaitToStop = 1000 * 10;
/// @brief How many times it should check to see if the generator is still running
const int MaxAttemptsToStopGenerator = 5;
/// @brief Max number of times the generator can try to start before it is considered a failure.
const int DefaultGeneratorTimesToStart = 3;
/// @brief The amount of time to wait between generator start attempts
const int DefaultDelayBetweenGeneratorStarts = 1000 * 5;
/// @brief Amount of time to wait to see if the generator started
const int DefaultGeneratorTimeToWaitForStart = 1000 * 10;

/// @brief How many milliseconds (ticks) required between messages. This is to prevent debounce.
const uint32_t MinTimeAllowedBetweenMessages = 500;

/// @brief Time in milliseconds to wait to confirm the utility is still off
const int DefaultUtilityOffWaitTime = 1000 * 1;

const int DefaultDelayUtilityOnWaitTime = 1000 * 2;

const int DefaultTimeToTriggerStart = 1000;
const int DefaultTimeToTriggerStop = 1000;

/// @brief denotes the generator on and off is momentary that it shouldn't stay on.
const bool GeneratorUsesMomentarySwitch = true;

