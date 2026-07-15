package com.emerald.app.ui.screens

import android.content.Context
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.grid.GridCells
import androidx.compose.foundation.lazy.grid.LazyVerticalGrid
import androidx.compose.foundation.lazy.grid.items
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Android
import androidx.compose.material.icons.filled.Apps
import androidx.compose.material.icons.filled.Business
import androidx.compose.material.icons.filled.DesktopWindows
import androidx.compose.material.icons.filled.Folder
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.Language
import androidx.compose.material.icons.filled.Payment
import androidx.compose.material.icons.filled.PhoneAndroid
import androidx.compose.material.icons.filled.Star
import androidx.compose.material.icons.filled.TravelExplore
import androidx.compose.material.icons.filled.VideogameAsset
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.emerald.app.ui.theme.EmeraldPrimary
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.File

// Map of app names to their icons and colors
data class AppIconInfo(
    val icon: ImageVector,
    val color: Color
)

private val appIcons = mapOf(
    "безіменна папка" to AppIconInfo(Icons.Default.Folder, Color(0xFFFFA726)),
    "безіменна папка 2" to AppIconInfo(Icons.Default.Folder, Color(0xFF66BB6A)),
    "грушик" to AppIconInfo(Icons.Default.Android, Color(0xFF42A5F5)),
    "чіча" to AppIconInfo(Icons.Default.VideogameAsset, Color(0xFFEF5350)),
    "e-suslya" to AppIconInfo(Icons.Default.Language, Color(0xFFAB47BC)),
    "Emerald Explorer" to AppIconInfo(Icons.Default.TravelExplore, Color(0xFF26A69A)),
    "EMERALD.inc" to AppIconInfo(Icons.Default.Business, Color(0xFF5C6BC0)),
    "epay" to AppIconInfo(Icons.Default.Payment, Color(0xFF66BB6A)),
    "ePhone" to AppIconInfo(Icons.Default.PhoneAndroid, Color(0xFF42A5F5)),
    "House Flopper" to AppIconInfo(Icons.Default.Star, Color(0xFFFFA726))
)

private val defaultAppIcon = AppIconInfo(Icons.Default.Apps, EmeraldPrimary)

@Composable
fun DesktopScreen(
    userEmail: String?,
    installedApps: Set<String>
) {
    var selectedApp by remember { mutableStateOf<String?>(null) }
    var selectedAppPath by remember { mutableStateOf<File?>(null) }
    val scope = rememberCoroutineScope()
    val context = LocalContext.current

    // If an app is selected, show WebView
    if (selectedApp != null && selectedAppPath != null) {
        AppWebView(
            appName = selectedApp!!,
            basePath = selectedAppPath!!,
            onClose = {
                selectedApp = null
                selectedAppPath = null
            }
        )
        return
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Top
    ) {
        Spacer(modifier = Modifier.height(8.dp))

        Text(
            text = "Робочий стіл",
            fontSize = 22.sp,
            fontWeight = FontWeight.Bold,
            color = EmeraldPrimary
        )

        Spacer(modifier = Modifier.height(4.dp))

        Text(
            text = "Ласкаво просимо, ${userEmail ?: "Користувач"}!",
            style = MaterialTheme.typography.bodyMedium,
            color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.7f)
        )

        Spacer(modifier = Modifier.height(16.dp))

        if (installedApps.isEmpty()) {
            // Empty desktop
            Card(
                modifier = Modifier.fillMaxWidth(),
                colors = CardDefaults.cardColors(
                    containerColor = MaterialTheme.colorScheme.surface
                ),
                elevation = CardDefaults.cardElevation(defaultElevation = 2.dp)
            ) {
                Column(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(32.dp),
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    Icon(
                        imageVector = Icons.Default.DesktopWindows,
                        contentDescription = null,
                        modifier = Modifier.size(64.dp),
                        tint = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.3f)
                    )

                    Spacer(modifier = Modifier.height(16.dp))

                    Text(
                        text = "Робочий стіл порожній",
                        style = MaterialTheme.typography.titleMedium,
                        fontWeight = FontWeight.SemiBold
                    )

                    Spacer(modifier = Modifier.height(8.dp))

                    Text(
                        text = "Встановіть додатки з розділу «Додатки», щоб вони з'явилися тут.",
                        style = MaterialTheme.typography.bodyMedium,
                        color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.7f),
                        textAlign = TextAlign.Center
                    )
                }
            }
        } else {
            // Grid of installed apps - like a real desktop
            LazyVerticalGrid(
                columns = GridCells.Fixed(4),
                horizontalArrangement = Arrangement.spacedBy(12.dp),
                verticalArrangement = Arrangement.spacedBy(16.dp),
                modifier = Modifier.fillMaxWidth()
            ) {
                items(installedApps.toList()) { appName ->
                    DesktopAppIcon(
                        appName = appName,
                        onClick = {
                            selectedApp = appName
                            scope.launch {
                                selectedAppPath = getProjectDir(context, appName)
                            }
                        }
                    )
                }
            }
        }

        Spacer(modifier = Modifier.weight(1f))

        // Account info card at the bottom
        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surface
            ),
            elevation = CardDefaults.cardElevation(defaultElevation = 2.dp)
        ) {
            Column(
                modifier = Modifier.padding(16.dp)
            ) {
                Text(
                    text = "Emerald Account",
                    style = MaterialTheme.typography.titleSmall,
                    fontWeight = FontWeight.SemiBold
                )

                Spacer(modifier = Modifier.height(4.dp))

                Text(
                    text = "Email: ${userEmail ?: "Невідомо"}",
                    style = MaterialTheme.typography.bodySmall
                )

                Spacer(modifier = Modifier.height(2.dp))

                Text(
                    text = "Статус: Активний",
                    style = MaterialTheme.typography.bodySmall,
                    color = EmeraldPrimary
                )
            }
        }
    }
}

@Composable
private fun DesktopAppIcon(
    appName: String,
    onClick: () -> Unit
) {
    val appIcon = appIcons[appName] ?: defaultAppIcon

    Column(
        modifier = Modifier
            .fillMaxWidth()
            .clickable(onClick = onClick),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        // App icon in a circle
        Box(
            modifier = Modifier
                .size(56.dp)
                .clip(CircleShape)
                .background(appIcon.color.copy(alpha = 0.15f)),
            contentAlignment = Alignment.Center
        ) {
            Icon(
                imageVector = appIcon.icon,
                contentDescription = appName,
                modifier = Modifier.size(32.dp),
                tint = appIcon.color
            )
        }

        Spacer(modifier = Modifier.height(6.dp))

        // App name below the icon
        Text(
            text = appName,
            style = MaterialTheme.typography.bodySmall,
            fontWeight = FontWeight.Medium,
            textAlign = TextAlign.Center,
            maxLines = 2,
            overflow = TextOverflow.Ellipsis,
            lineHeight = 14.sp,
            modifier = Modifier.fillMaxWidth()
        )
    }
}

private suspend fun getProjectDir(context: Context, projectName: String): File? {
    return withContext(Dispatchers.IO) {
        val projectDir = File(context.getExternalFilesDir(null), "Desktop/$projectName")
        if (projectDir.exists() && projectDir.isDirectory) projectDir else null
    }
}
