#pragma once

/// @brief Time for the generator to warm up in milliseconds
const int DefaultGeneratorWarmUpTime = 1000 * 60;
const int DefaultGeneratorCoolDownTime = 1000 * 60;
const int DefaultGeneratorTimeToWaitToStop = 4000;
const int MaxAttemptsToStopGenerator = 3;
/// @brief Max number of times the generator can try to start before it is considered a failure.
const int DefaultGeneratorTimesToStart = 3;
/// @brief The amount of time to wait between generator start attempts
const int DefaultDelayBetweenGeneratorStarts = 2000;
/// @brief Amount of time to wait to see if the generator started
const int DefaultGeneratorTimeToWaitForStart = 1000 * 10;

/// @brief How many milliseconds (ticks) required between messages. This is to prevent debounce.
const uint32_t MinTimeAllowedBetweenMessages = 500;

/// @brief Time in milliseconds to wait to confirm the utility is still off
const int DefaultUtilityOffWaitTime = 2000;

const int DefaultDelayUtilityOnWaitTime = 2000;

