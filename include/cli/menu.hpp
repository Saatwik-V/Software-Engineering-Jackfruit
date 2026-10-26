/**
 * @file menu.hpp
 * @brief Interactive terminal navigation menu shell (JACK-F-001, JACK-NF-005).
 *
 * Implements an interactive console interface supporting numbered navigation,
 * path validation, colored status messages, and routing to compression,
 * decompression, archive statistics, and integrity verification modules.
 */

#pragma once

#include <iostream>
#include <string>

namespace jackfruit::cli {

/**
 * @brief Interactive CLI menu shell managing console user interaction.
 */
class Menu {
public:
    Menu() = default;
    ~Menu() = default;

    /**
     * @brief Launches the main interactive menu event loop.
     * @param in Input stream (defaults to std::cin).
     * @param out Output stream (defaults to std::cout).
     */
    void run(std::istream& in = std::cin, std::ostream& out = std::cout);

    /**
     * @brief Displays the formatted menu header and numbered options with ANSI colors.
     * @param out Output stream.
     */
    void displayMenu(std::ostream& out) const;

    /**
     * @brief Handles a single user choice.
     * @param choice Numbered menu option (1-5).
     * @param in Input stream for sub-prompts.
     * @param out Output stream for prompts and results.
     * @return true to continue the loop, false to exit cleanly.
     */
    bool handleChoice(int choice, std::istream& in, std::ostream& out);

    /**
     * @brief Handles Option 1: File compression prompt, validation, and execution.
     * @param in Input stream.
     * @param out Output stream.
     */
    void handleCompress(std::istream& in, std::ostream& out);

    /**
     * @brief Handles Option 2: Archive decompression prompt and execution.
     * @param in Input stream.
     * @param out Output stream.
     */
    void handleDecompress(std::istream& in, std::ostream& out);

    /**
     * @brief Handles Option 3: Archive metadata and compression statistics display.
     * @param in Input stream.
     * @param out Output stream.
     */
    void handleStatistics(std::istream& in, std::ostream& out);

    /**
     * @brief Handles Option 4: Archive integrity validation and CRC-32 check.
     * @param in Input stream.
     * @param out Output stream.
     */
    void handleValidate(std::istream& in, std::ostream& out);

    /**
     * @brief Enable or disable ANSI color formatting (e.g. for plain streams/tests).
     * @param enabled True to enable ANSI colors, false for plain text.
     */
    void setColored(bool enabled) { useColor_ = enabled; }

    /**
     * @brief Checks if color formatting is currently enabled.
     * @return True if colored, false otherwise.
     */
    bool isColored() const { return useColor_; }

private:
    bool useColor_{true};

    // ANSI color helper methods
    std::string colorBold() const;
    std::string colorCyan() const;
    std::string colorGreen() const;
    std::string colorYellow() const;
    std::string colorRed() const;
    std::string colorReset() const;
};

} // namespace jackfruit::cli
