#include <gtest/gtest.h>
#include <game_manager.h>

// Sample Test
TEST(GameManagerTest, TestInitialization) {
    GameManager gameManager;
    EXPECT_EQ(gameManager.isGameOver(), false);
}