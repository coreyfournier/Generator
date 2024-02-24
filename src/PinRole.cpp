#pragma once

enum PinRole : int{

    /// @brief Start the generator
    Start = 0,
    /// @brief Stop the generator
    Stop = 1,
    /// @brief Fire the transfer switch
    Transfer = 2,
    /// @brief Leg 1 of the utility supply
    UtilityOnL1 = 3,
    /// @brief Leg 2 of the utility supply
    UtilityOnL2 = 4,
    /// @brief Leg 1 of the utility supply
    GeneratorOnL1 = 5,
    /// @brief Leg 2 of the utility supply
    GeneratorOnL2 = 6,
    /// @brief Some un documented use case, that can be ignored if desired
    Other = 7
};