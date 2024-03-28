#pragma once

/// @brief Time for the generator to warm up in milliseconds. It appears it doesn't enable large load devices to run initially, so this can be a short duration.
const int DefaultGeneratorWarmUpTime = 1000 * 10;
const int DefaultGeneratorCoolDownTime = 1000 * 60;
const int DefaultGeneratorTimeToWaitToStop = 1000 * 4;
const int MaxAttemptsToStopGenerator = 4;
/// @brief Max number of times the generator can try to start before it is considered a failure.
const int DefaultGeneratorTimesToStart = 3;
/// @brief The amount of time to wait between generator start attempts
const int DefaultDelayBetweenGeneratorStarts = 1000 * 5;
/// @brief Amount of time to wait to see if the generator started
const int DefaultGeneratorTimeToWaitForStart = 1000 * 10;

/// @brief How many milliseconds (ticks) required between messages. This is to prevent debounce.
const uint32_t MinTimeAllowedBetweenMessages = 500;

/// @brief Time in milliseconds to wait to confirm the utility is still off
const int DefaultUtilityOffWaitTime = 1000 * 2;

const int DefaultDelayUtilityOnWaitTime = 1000 * 2;

const int DefaultTimeToTriggerStart = 1000;

