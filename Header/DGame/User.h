/**
 * User.h - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

namespace DGame {

/**
 * @brief Holds references to user related interfaces
 */
struct User
{
  /**
   * @brief Accesses application related data stored on the users filesystem
   * @note Unix: ~/.local/share/<Company>/<AppName>
   * @note Windows: %localappdata%/<Company>/<AppName>
   */
  struct AppData
  {};

  struct Audio
  {};

  const AppData Data;
  const Audio Sound;

  User(const User &) = delete;
  void operator=(const User &) = delete;

  static User &
  Instance()
  {
    static User instance;
    return instance;
  }

private:
  User()
  : Data{}
  , Sound{}
  {}

  ~User()
  {}
};
} // namespace DGame
