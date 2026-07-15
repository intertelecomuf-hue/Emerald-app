package com.emerald.app.ui.screens

import android.annotation.SuppressLint
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri
import android.view.ViewGroup
import android.webkit.WebChromeClient
import android.webkit.WebResourceRequest
import android.webkit.WebView
import android.webkit.WebViewClient
import androidx.compose.foundation.layout.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowBack
import androidx.compose.material.icons.filled.Code
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.viewinterop.AndroidView
import com.emerald.app.ui.theme.EmeraldPrimary
import java.io.File

@SuppressLint("SetJavaScriptEnabled")
@Composable
fun AppWebView(
    appName: String,
    basePath: File,
    onClose: () -> Unit
) {
    val context = LocalContext.current
    var webView by remember { mutableStateOf<WebView?>(null) }
    var currentTitle by remember { mutableStateOf(appName) }
    var canGoBack by remember { mutableStateOf(false) }

    // Find index.html or first HTML file
    val startFile = remember {
        findStartHtmlFile(basePath)
    }

    var showNoHtmlMessage by remember { mutableStateOf(startFile == null) }

    Column(
        modifier = Modifier.fillMaxSize()
    ) {
        // Top bar with navigation
        TopAppBar(
            title = {
                Column {
                    Text(
                        text = currentTitle,
                        fontSize = 18.sp,
                        fontWeight = FontWeight.Bold,
                        maxLines = 1
                    )
                    Text(
                        text = appName,
                        fontSize = 12.sp,
                        color = MaterialTheme.colorScheme.onPrimary.copy(alpha = 0.7f),
                        maxLines = 1
                    )
                }
            },
            navigationIcon = {
                IconButton(onClick = {
                    if (canGoBack) {
                        webView?.goBack()
                    } else {
                        onClose()
                    }
                }) {
                    Icon(
                        imageVector = Icons.Default.ArrowBack,
                        contentDescription = "Назад"
                    )
                }
            },
            actions = {
                IconButton(onClick = { webView?.reload() }) {
                    Icon(
                        imageVector = Icons.Default.Refresh,
                        contentDescription = "Оновити"
                    )
                }
                TextButton(onClick = onClose) {
                    Text(
                        "Закрити",
                        color = MaterialTheme.colorScheme.onPrimary
                    )
                }
            },
            colors = TopAppBarDefaults.topAppBarColors(
                containerColor = EmeraldPrimary,
                titleContentColor = MaterialTheme.colorScheme.onPrimary,
                navigationIconContentColor = MaterialTheme.colorScheme.onPrimary
            )
        )

        if (showNoHtmlMessage) {
            // Show message when no HTML file is found
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(32.dp),
                contentAlignment = Alignment.Center
            ) {
                Column(
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    Icon(
                        imageVector = Icons.Default.Code,
                        contentDescription = null,
                        modifier = Modifier.size(64.dp),
                        tint = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.3f)
                    )

                    Spacer(modifier = Modifier.height(16.dp))

                    Text(
                        text = "Цей додаток не має веб-інтерфейсу",
                        style = MaterialTheme.typography.titleMedium,
                        fontWeight = FontWeight.SemiBold,
                        textAlign = androidx.compose.ui.text.style.TextAlign.Center
                    )

                    Spacer(modifier = Modifier.height(8.dp))

                    Text(
                        text = "У проекті \"$appName\" не знайдено HTML-файлів для відображення.",
                        style = MaterialTheme.typography.bodyMedium,
                        color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.7f),
                        textAlign = androidx.compose.ui.text.style.TextAlign.Center
                    )
                }
            }
        } else {
            // WebView
            AndroidView(
                factory = { ctx ->
                    WebView(ctx).apply {
                        layoutParams = ViewGroup.LayoutParams(
                            ViewGroup.LayoutParams.MATCH_PARENT,
                            ViewGroup.LayoutParams.MATCH_PARENT
                        )

                        settings.javaScriptEnabled = true
                        settings.domStorageEnabled = true
                        settings.allowFileAccess = true
                        settings.allowContentAccess = true
                        settings.setSupportZoom(true)
                        settings.builtInZoomControls = true
                        settings.displayZoomControls = false
                        settings.loadWithOverviewMode = true
                        settings.useWideViewPort = true
                        settings.allowFileAccessFromFileURLs = true
                        settings.allowUniversalAccessFromFileURLs = true

                        webViewClient = object : WebViewClient() {
                            override fun onPageStarted(view: WebView?, url: String?, favicon: Bitmap?) {
                                super.onPageStarted(view, url, favicon)
                            }

                            override fun onPageFinished(view: WebView?, url: String?) {
                                super.onPageFinished(view, url)
                                canGoBack = view?.canGoBack() ?: false
                            }

                            override fun shouldOverrideUrlLoading(
                                view: WebView?,
                                request: WebResourceRequest?
                            ): Boolean {
                                val url = request?.url ?: return false
                                val urlStr = url.toString()

                                // If it's an external link (http/https), open in browser
                                if (urlStr.startsWith("http://") || urlStr.startsWith("https://")) {
                                    val intent = Intent(Intent.ACTION_VIEW, url)
                                    ctx.startActivity(intent)
                                    return true
                                }

                                // Allow internal navigation for file:// URLs
                                return false
                            }
                        }

                        webChromeClient = object : WebChromeClient() {
                            override fun onReceivedTitle(view: WebView?, title: String?) {
                                super.onReceivedTitle(view, title)
                                if (!title.isNullOrBlank()) {
                                    currentTitle = title
                                }
                            }
                        }

                        // Load the start file
                        if (startFile != null) {
                            loadUrl("file://${startFile.absolutePath}")
                        }

                        webView = this
                    }
                },
                modifier = Modifier.fillMaxSize()
            )
        }
    }
}

/**
 * Find the best HTML file to open in the project directory.
 * Prefers index.html, then any other .html file.
 */
private fun findStartHtmlFile(projectDir: File): File? {
    if (!projectDir.exists() || !projectDir.isDirectory) return null

    // First, look for index.html in root
    val indexHtml = File(projectDir, "index.html")
    if (indexHtml.exists()) return indexHtml

    // Look for any .html file in root
    val htmlFiles = projectDir.listFiles { file ->
        file.isFile && file.name.endsWith(".html", ignoreCase = true)
    }
    if (htmlFiles != null && htmlFiles.isNotEmpty()) {
        return htmlFiles.first()
    }

    // Look for index.html in subdirectories (one level deep)
    val subDirs = projectDir.listFiles { it.isDirectory }
    if (subDirs != null) {
        for (dir in subDirs) {
            val subIndex = File(dir, "index.html")
            if (subIndex.exists()) return subIndex
        }
    }

    return null
}
