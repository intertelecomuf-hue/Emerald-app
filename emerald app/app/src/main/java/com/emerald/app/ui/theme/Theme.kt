package com.emerald.app.ui.theme

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable

private val EmeraldColorScheme = lightColorScheme(
    primary = EmeraldPrimary,
    onPrimary = EmeraldOnPrimary,
    primaryContainer = EmeraldPrimaryLight,
    secondary = EmeraldAccent,
    background = EmeraldBackground,
    surface = EmeraldSurface,
    onBackground = EmeraldOnBackground,
    onSurface = EmeraldOnSurface,
    error = EmeraldError,
)

@Composable
fun EmeraldTheme(content: @Composable () -> Unit) {
    MaterialTheme(
        colorScheme = EmeraldColorScheme,
        content = content
    )
}
