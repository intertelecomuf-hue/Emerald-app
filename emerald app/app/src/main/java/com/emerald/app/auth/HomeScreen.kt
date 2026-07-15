package com.emerald.app.auth

import androidx.compose.foundation.layout.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AccountCircle
import androidx.compose.material.icons.filled.Apps
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.SmartToy
import androidx.compose.material.icons.outlined.AccountCircle
import androidx.compose.material.icons.outlined.Apps
import androidx.compose.material.icons.outlined.Home
import androidx.compose.material.icons.outlined.SmartToy
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import com.emerald.app.ui.screens.AccountScreen
import com.emerald.app.ui.screens.AppsScreen
import com.emerald.app.ui.screens.BotScreen
import com.emerald.app.ui.screens.DesktopScreen
import com.emerald.app.ui.theme.EmeraldPrimary

enum class BottomNavItem(
    val label: String,
    val selectedIcon: ImageVector,
    val unselectedIcon: ImageVector
) {
    DESKTOP("Робочий стіл", Icons.Filled.Home, Icons.Outlined.Home),
    APPS("Додатки", Icons.Filled.Apps, Icons.Outlined.Apps),
    BOT("BotShe", Icons.Filled.SmartToy, Icons.Outlined.SmartToy),
    ACCOUNT("Акаунт", Icons.Filled.AccountCircle, Icons.Outlined.AccountCircle)
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun HomeScreen(
    authManager: AuthManager,
    onLogout: () -> Unit
) {
    var selectedTab by remember { mutableStateOf(BottomNavItem.DESKTOP) }
    var installedApps by remember { mutableStateOf(setOf<String>()) }
    val userEmail = authManager.currentUserEmail

    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    Text(
                        when (selectedTab) {
                            BottomNavItem.DESKTOP -> "Emerald"
                            BottomNavItem.APPS -> "Додатки"
                            BottomNavItem.BOT -> "Emerald Bot"
                            BottomNavItem.ACCOUNT -> "Акаунт"
                        }
                    )
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = EmeraldPrimary,
                    titleContentColor = MaterialTheme.colorScheme.onPrimary
                )
            )
        },
        bottomBar = {
            NavigationBar {
                BottomNavItem.entries.forEach { item ->
                    NavigationBarItem(
                        icon = {
                            Icon(
                                imageVector = if (selectedTab == item) item.selectedIcon else item.unselectedIcon,
                                contentDescription = item.label
                            )
                        },
                        label = { Text(item.label) },
                        selected = selectedTab == item,
                        onClick = { selectedTab = item },
                        colors = NavigationBarItemDefaults.colors(
                            selectedIconColor = EmeraldPrimary,
                            selectedTextColor = EmeraldPrimary,
                            indicatorColor = EmeraldPrimary.copy(alpha = 0.1f)
                        )
                    )
                }
            }
        }
    ) { paddingValues ->
        Box(
            modifier = Modifier
                .fillMaxSize()
                .padding(paddingValues)
        ) {
            when (selectedTab) {
                BottomNavItem.DESKTOP -> DesktopScreen(
                    userEmail = userEmail,
                    installedApps = installedApps
                )
                BottomNavItem.APPS -> AppsScreen(
                    installedApps = installedApps,
                    onInstallApp = { appName ->
                        installedApps = installedApps + appName
                    }
                )
                BottomNavItem.BOT -> BotScreen()
                BottomNavItem.ACCOUNT -> AccountScreen(
                    authManager = authManager,
                    onLogout = onLogout
                )
            }
        }
    }
}
