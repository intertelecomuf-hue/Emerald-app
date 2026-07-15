package com.emerald.app.navigation

import androidx.compose.runtime.Composable
import androidx.navigation.NavHostController
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import com.emerald.app.auth.AuthManager
import com.emerald.app.auth.HomeScreen

object Routes {
    const val HOME = "home"
}

@Composable
fun EmeraldNavGraph(
    navController: NavHostController
) {
    NavHost(
        navController = navController,
        startDestination = Routes.HOME
    ) {
        composable(Routes.HOME) {
            HomeScreen(
                authManager = AuthManager,
                onLogout = {
                    AuthManager.logout()
                }
            )
        }
    }
}
