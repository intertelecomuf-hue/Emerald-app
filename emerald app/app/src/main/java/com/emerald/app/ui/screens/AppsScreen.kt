package com.emerald.app.ui.screens

import android.content.Context
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Apps
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.filled.Folder
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.emerald.app.ui.theme.EmeraldPrimary
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.File
import java.io.FileOutputStream

data class ProjectItem(
    val name: String
)

@Composable
fun AppsScreen(
    installedApps: Set<String>,
    onInstallApp: (String) -> Unit
) {
    val projects = remember {
        getProjectsList()
    }

    var statusMessage by remember { mutableStateOf<String?>(null) }
    val scope = rememberCoroutineScope()
    val context = LocalContext.current

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(24.dp),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Top
    ) {
        Spacer(modifier = Modifier.height(16.dp))

        Icon(
            imageVector = Icons.Default.Apps,
            contentDescription = null,
            modifier = Modifier.size(64.dp),
            tint = EmeraldPrimary
        )

        Spacer(modifier = Modifier.height(16.dp))

        Text(
            text = "Додатки",
            fontSize = 24.sp,
            fontWeight = FontWeight.Bold,
            color = EmeraldPrimary
        )

        Spacer(modifier = Modifier.height(8.dp))

        Text(
            text = "Встановіть додаток на робочий стіл",
            style = MaterialTheme.typography.bodyLarge,
            color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.7f)
        )

        Spacer(modifier = Modifier.height(24.dp))

        if (statusMessage != null) {
            Card(
                colors = CardDefaults.cardColors(
                    containerColor = EmeraldPrimary.copy(alpha = 0.1f)
                ),
                modifier = Modifier.fillMaxWidth()
            ) {
                Text(
                    text = statusMessage!!,
                    modifier = Modifier.padding(12.dp),
                    style = MaterialTheme.typography.bodyMedium,
                    color = EmeraldPrimary
                )
            }
            Spacer(modifier = Modifier.height(12.dp))
        }

        LazyColumn(
            verticalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            items(projects) { project ->
                ProjectCard(
                    project = project,
                    isInstalled = project.name in installedApps,
                    onInstall = {
                        scope.launch {
                            statusMessage = "Встановлення ${project.name}..."
                            val result = copyProjectToDesktop(context, project.name)
                            if (result) {
                                onInstallApp(project.name)
                                statusMessage = "${project.name} встановлено на робочий стіл!"
                            } else {
                                statusMessage = "Помилка встановлення ${project.name}"
                            }
                        }
                    }
                )
            }
        }
    }
}

@Composable
private fun ProjectCard(
    project: ProjectItem,
    isInstalled: Boolean,
    onInstall: () -> Unit
) {
    Card(
        modifier = Modifier
            .fillMaxWidth()
            .clickable(enabled = !isInstalled) { onInstall() },
        colors = CardDefaults.cardColors(
            containerColor = if (isInstalled)
                EmeraldPrimary.copy(alpha = 0.05f)
            else
                MaterialTheme.colorScheme.surface
        ),
        elevation = CardDefaults.cardElevation(defaultElevation = 2.dp)
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            Icon(
                imageVector = if (isInstalled) Icons.Default.CheckCircle else Icons.Default.Folder,
                contentDescription = null,
                tint = if (isInstalled) EmeraldPrimary else MaterialTheme.colorScheme.onSurface.copy(alpha = 0.6f),
                modifier = Modifier.size(32.dp)
            )

            Spacer(modifier = Modifier.width(12.dp))

            Column(modifier = Modifier.weight(1f)) {
                Text(
                    text = project.name,
                    style = MaterialTheme.typography.titleMedium,
                    fontWeight = FontWeight.SemiBold
                )
                Text(
                    text = if (isInstalled) "Встановлено на робочий стіл" else "Натисніть, щоб встановити",
                    style = MaterialTheme.typography.bodySmall,
                    color = if (isInstalled) EmeraldPrimary
                    else MaterialTheme.colorScheme.onSurface.copy(alpha = 0.5f)
                )
            }
        }
    }
}

private suspend fun copyProjectToDesktop(context: Context, projectName: String): Boolean {
    return withContext(Dispatchers.IO) {
        try {
            val desktopDir = File(
                context.getExternalFilesDir(null),
                "Desktop"
            )
            if (!desktopDir.exists()) desktopDir.mkdirs()

            val projectDir = File(desktopDir, projectName)
            if (projectDir.exists()) projectDir.deleteRecursively()
            projectDir.mkdirs()

            // Copy files from assets to desktop
            val assetPath = "projects/$projectName"
            val assetManager = context.assets
            val files = assetManager.list(assetPath)

            if (files != null) {
                copyAssetsRecursively(context, assetPath, projectDir)
            }

            true
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }
}

private fun copyAssetsRecursively(context: Context, assetPath: String, destDir: File) {
    val assetManager = context.assets
    val files = assetManager.list(assetPath) ?: return

    for (fileName in files) {
        val childAssetPath = "$assetPath/$fileName"
        val childFile = File(destDir, fileName)

        try {
            val subFiles = assetManager.list(childAssetPath)
            if (subFiles != null && subFiles.isNotEmpty()) {
                // It's a directory
                childFile.mkdirs()
                copyAssetsRecursively(context, childAssetPath, childFile)
            } else {
                // It's a file
                childFile.parentFile?.mkdirs()
                val inputStream = assetManager.open(childAssetPath)
                val outputStream = FileOutputStream(childFile)
                inputStream.copyTo(outputStream)
                inputStream.close()
                outputStream.close()
            }
        } catch (e: Exception) {
            // Skip files that can't be read
        }
    }
}

private fun getProjectsList(): List<ProjectItem> {
    return listOf(
        ProjectItem(name = "безіменна папка"),
        ProjectItem(name = "безіменна папка 2"),
        ProjectItem(name = "грушик"),
        ProjectItem(name = "чіча"),
        ProjectItem(name = "e-suslya"),
        ProjectItem(name = "Emerald Explorer"),
        ProjectItem(name = "EMERALD.inc"),
        ProjectItem(name = "epay"),
        ProjectItem(name = "ePhone"),
        ProjectItem(name = "House Flopper")
    )
}
