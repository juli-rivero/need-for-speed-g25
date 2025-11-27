#include "client/qt/theme_manager.h"

ThemeManager::ThemeManager() : currentTheme(ThemeType::Default) {
    currentPalette = createDefaultPalette();
}

void ThemeManager::setTheme(ThemeType type) {
    currentTheme = type;

    switch (type) {
        case ThemeType::Default:
            currentPalette = createDefaultPalette();
            break;
        case ThemeType::DarkMode:
            currentPalette = createDarkModePalette();
            break;
        case ThemeType::RacingRed:
            currentPalette = createRacingRedPalette();
            break;
        case ThemeType::Luxury:
            currentPalette = createLuxuryPalette();
            break;
        case ThemeType::OceanBlue:
            currentPalette = createOceanBluePalette();
            break;
        case ThemeType::PostApocalypse:
            currentPalette = createPostApocalypsePalette();
            break;
    }

    emit themeChanged();
}

QString ThemeManager::getThemeName(ThemeType type) const {
    switch (type) {
        case ThemeType::Default:
            return "🎨 Material Design";
        case ThemeType::DarkMode:
            return "🌙 Dark Mode";
        case ThemeType::RacingRed:
            return "🔥 Racing Red";
        case ThemeType::Luxury:
            return "💎 Luxury";
        case ThemeType::OceanBlue:
            return "🌊 Ocean Blue";
        case ThemeType::PostApocalypse:
            return "☢️ Post-Apocalypse";
    }
    return "Unknown";
}

// Funciones de estilo
QString ThemeManager::buttonPrimaryStyle() const {
    return QString(
               "QPushButton {"
               "  background-color: %1;"
               "  color: %2;"
               "  border: none;"
               "  border-radius: %3px;"
               "  padding: 10px 20px;"
               "  font-size: 14px;"
               "  font-weight: bold;"
               "}"
               "QPushButton:hover {"
               "  background-color: %4;"
               "}"
               "QPushButton:disabled {"
               "  background-color: %5;"
               "}")
        .arg(currentPalette.primaryColor)
        .arg(currentPalette.textLight)
        .arg(BORDER_RADIUS)
        .arg(currentPalette.primaryHover)
        .arg(currentPalette.disabledBackground);
}

QString ThemeManager::buttonSecondaryStyle() const {
    return QString(
               "QPushButton {"
               "  background-color: %1;"
               "  color: %2;"
               "  border: none;"
               "  border-radius: %3px;"
               "  padding: 10px 20px;"
               "  font-size: 14px;"
               "  font-weight: bold;"
               "}"
               "QPushButton:hover {"
               "  background-color: %4;"
               "}"
               "QPushButton:disabled {"
               "  background-color: %5;"
               "}")
        .arg(currentPalette.secondaryColor)
        .arg(currentPalette.textLight)
        .arg(BORDER_RADIUS)
        .arg(currentPalette.secondaryHover)
        .arg(currentPalette.disabledBackground);
}

QString ThemeManager::buttonActionStyle() const {
    return QString(
               "QPushButton {"
               "  background-color: %1;"
               "  color: %2;"
               "  border: none;"
               "  border-radius: %3px;"
               "  padding: 10px 20px;"
               "  font-size: 14px;"
               "  font-weight: bold;"
               "}"
               "QPushButton:hover {"
               "  background-color: %4;"
               "}")
        .arg(currentPalette.actionColor)
        .arg(currentPalette.textLight)
        .arg(BORDER_RADIUS)
        .arg(currentPalette.actionHover);
}

QString ThemeManager::buttonDangerStyle() const {
    return QString(
               "QPushButton {"
               "  background-color: %1;"
               "  color: %2;"
               "  border: none;"
               "  border-radius: %3px;"
               "  padding: 10px 20px;"
               "  font-size: 14px;"
               "  font-weight: bold;"
               "}"
               "QPushButton:hover {"
               "  background-color: %4;"
               "}")
        .arg(currentPalette.dangerColor)
        .arg(currentPalette.textLight)
        .arg(BORDER_RADIUS)
        .arg(currentPalette.dangerHover);
}

QString ThemeManager::gameCardStyle(bool selected) const {
    return QString(
               "QFrame {"
               "    background-color: %1;"
               "    border: 2px solid %2;"
               "    border-radius: %3px;"
               "    padding: 15px;"
               "    margin: 5px;"
               "}"
               "QFrame:hover {"
               "    border-color: %4;"
               "    background-color: %5;"
               "}")
        .arg(currentPalette.cardBackground)
        .arg(selected ? currentPalette.borderSelected
                      : currentPalette.borderColor)
        .arg(CARD_BORDER_RADIUS)
        .arg(currentPalette.borderSelected)
        .arg(currentPalette.cardBackgroundHover);
}

QString ThemeManager::progressBarStyle(const QString& fillColor) const {
    QString fill =
        fillColor.isEmpty() ? currentPalette.progressFill : fillColor;

    return QString(
               "QProgressBar {"
               "    border: 1px solid %1;"
               "    border-radius: 4px;"
               "    background-color: %2;"
               "    text-align: center;"
               "}"
               "QProgressBar::chunk {"
               "    background-color: %3;"
               "    border-radius: 3px;"
               "}")
        .arg(currentPalette.borderColor)
        .arg(currentPalette.progressBackground)
        .arg(fill);
}

QString ThemeManager::statusSuccessStyle() const {
    return QString("color: %1;").arg(currentPalette.statusSuccess);
}

QString ThemeManager::statusWarningStyle() const {
    return QString("color: %1;").arg(currentPalette.statusWarning);
}

QString ThemeManager::statusErrorStyle() const {
    return QString("color: %1;").arg(currentPalette.statusError);
}

QString ThemeManager::carCardStyle() const {
    return QString(
               "QWidget {"
               "    background-color: %1;"
               "    border: 2px solid %2;"
               "    border-radius: 8px;"
               "    padding: 10px;"
               "    margin: 5px;"
               "}"
               "QWidget:hover {"
               "    background-color: %3;"
               "    border-color: %4;"
               "}")
        .arg(currentPalette.cardBackground)
        .arg(currentPalette.borderColor)
        .arg(currentPalette.cardBackgroundHover)
        .arg(currentPalette.primaryColor);
}

QString ThemeManager::carStatBarStyle(float value) const {
    // Elegir color basado en el valor
    QString barColor;
    if (value >= 0.8f) {
        barColor = currentPalette.statusSuccess;  // Verde para valores altos
    } else if (value >= 0.6f) {
        barColor = "#8BC34A";  // Verde claro
    } else if (value >= 0.4f) {
        barColor = currentPalette.statusWarning;  // Naranja para valores medios
    } else {
        barColor = currentPalette.statusError;  // Rojo para valores bajos
    }

    return QString(
               "QProgressBar {"
               "    border: 1px solid %1;"
               "    border-radius: 4px;"
               "    background-color: %2;"
               "    text-align: center;"
               "    color: %3;"
               "    font-weight: bold;"
               "}"
               "QProgressBar::chunk {"
               "    background-color: %4;"
               "    border-radius: 3px;"
               "}")
        .arg(currentPalette.borderColor)
        .arg(currentPalette.progressBackground)
        .arg(currentPalette.textPrimary)
        .arg(barColor);
}

// Creación de paletas
ColorPalette ThemeManager::createDefaultPalette() {
    ColorPalette palette;

    // Colores primarios - Material Design
    palette.primaryColor = "#4CAF50";
    palette.primaryHover = "#45a049";
    palette.secondaryColor = "#2196F3";
    palette.secondaryHover = "#0b7dda";
    palette.actionColor = "#ff9800";
    palette.actionHover = "#e68900";
    palette.dangerColor = "#f44336";
    palette.dangerHover = "#da190b";

    // Fondos
    palette.cardBackground = "#ffffff";
    palette.cardBackgroundHover = "#f9f9f9";
    palette.borderColor = "#dddddd";
    palette.borderSelected = "#4CAF50";

    // Textos
    palette.textPrimary = "#000000";
    palette.textSecondary = "#333333";
    palette.textLight = "#ffffff";
    palette.textBackground = "#ffffff";

    // Estados
    palette.statusSuccess = "#4CAF50";
    palette.statusWarning = "#ff9800";
    palette.statusError = "#f44336";

    // Barras
    palette.progressBackground = "#f0f0f0";
    palette.progressFill = "#4CAF50";

    // Stats
    palette.statSpeed = "#FF5722";
    palette.statAcceleration = "#FF9800";
    palette.statHealth = "#4CAF50";
    palette.statMass = "#9C27B0";
    palette.statControl = "#2196F3";

    // Estados partida
    palette.stateWaiting = "#4CAF50";
    palette.statePlaying = "#ff9800";
    palette.stateFull = "#9e9e9e";

    // Deshabilitado
    palette.disabledBackground = "#cccccc";
    palette.disabledText = "#666666";

    return palette;
}

ColorPalette ThemeManager::createDarkModePalette() {
    ColorPalette palette;

    // Colores primarios - Cyberpunk
    palette.primaryColor = "#BB86FC";
    palette.primaryHover = "#9965F4";
    palette.secondaryColor = "#03DAC6";
    palette.secondaryHover = "#018786";
    palette.actionColor = "#CF6679";
    palette.actionHover = "#B74F64";
    palette.dangerColor = "#CF6679";
    palette.dangerHover = "#B74F64";

    // Fondos
    palette.cardBackground = "#1E1E1E";
    palette.cardBackgroundHover = "#2D2D2D";
    palette.borderColor = "#333333";
    palette.borderSelected = "#BB86FC";

    // Textos - CORREGIDO: Cambiar textBackground por un color claro
    palette.textPrimary = "#FFFFFF";
    palette.textSecondary = "#E0E0E0";
    palette.textLight = "#FFFFFF";
    palette.textBackground = "#FFFFFF";  // Cambiado de #000000 a #FFFFFF

    // Estados
    palette.statusSuccess = "#03DAC6";
    palette.statusWarning = "#CF6679";
    palette.statusError = "#CF6679";

    // Barras
    palette.progressBackground = "#2D2D2D";
    palette.progressFill = "#BB86FC";

    // Stats
    palette.statSpeed = "#CF6679";
    palette.statAcceleration = "#FF9800";
    palette.statHealth = "#03DAC6";
    palette.statMass = "#BB86FC";
    palette.statControl = "#03DAC6";

    // Estados partida
    palette.stateWaiting = "#03DAC6";
    palette.statePlaying = "#CF6679";
    palette.stateFull = "#666666";

    // Deshabilitado
    palette.disabledBackground = "#333333";
    palette.disabledText = "#666666";

    return palette;
}

ColorPalette ThemeManager::createRacingRedPalette() {
    ColorPalette palette;

    // Colores primarios - Ferrari
    palette.primaryColor = "#E63946";
    palette.primaryHover = "#D62828";
    palette.secondaryColor = "#FFD60A";
    palette.secondaryHover = "#FFC300";
    palette.actionColor = "#457B9D";
    palette.actionHover = "#1D3557";
    palette.dangerColor = "#780000";
    palette.dangerHover = "#560000";

    // Fondos
    palette.cardBackground = "#16213E";
    palette.cardBackgroundHover = "#1A2742";
    palette.borderColor = "#2E3D5E";
    palette.borderSelected = "#E63946";

    // Textos
    palette.textPrimary = "#F1F1F1";
    palette.textSecondary = "#A8DADC";
    palette.textLight = "#FFFFFF";

    // Estados
    palette.statusSuccess = "#FFD60A";
    palette.statusWarning = "#FF9800";
    palette.statusError = "#E63946";

    // Barras
    palette.progressBackground = "#2E3D5E";
    palette.progressFill = "#E63946";

    // Stats
    palette.statSpeed = "#E63946";
    palette.statAcceleration = "#FFD60A";
    palette.statHealth = "#457B9D";
    palette.statMass = "#1D3557";
    palette.statControl = "#FFD60A";

    // Estados partida
    palette.stateWaiting = "#FFD60A";
    palette.statePlaying = "#E63946";
    palette.stateFull = "#6C757D";

    // Deshabilitado
    palette.disabledBackground = "#2E3D5E";
    palette.disabledText = "#6C757D";

    return palette;
}

ColorPalette ThemeManager::createLuxuryPalette() {
    ColorPalette palette;

    // Colores primarios - Premium
    palette.primaryColor = "#D4AF37";
    palette.primaryHover = "#C5A028";
    palette.secondaryColor = "#2E4057";
    palette.secondaryHover = "#1F2D3D";
    palette.actionColor = "#8B4513";
    palette.actionHover = "#654321";
    palette.dangerColor = "#8B0000";
    palette.dangerHover = "#660000";

    // Fondos
    palette.cardBackground = "#2E2E2E";
    palette.cardBackgroundHover = "#3A3A3A";
    palette.borderColor = "#4A4A4A";
    palette.borderSelected = "#D4AF37";

    // Textos
    palette.textPrimary = "#ECECEC";
    palette.textSecondary = "#B8B8B8";
    palette.textLight = "#FFFFFF";

    // Estados
    palette.statusSuccess = "#D4AF37";
    palette.statusWarning = "#8B4513";
    palette.statusError = "#8B0000";

    // Barras
    palette.progressBackground = "#3A3A3A";
    palette.progressFill = "#D4AF37";

    // Stats
    palette.statSpeed = "#8B0000";
    palette.statAcceleration = "#D4AF37";
    palette.statHealth = "#2E4057";
    palette.statMass = "#8B4513";
    palette.statControl = "#D4AF37";

    // Estados partida
    palette.stateWaiting = "#D4AF37";
    palette.statePlaying = "#8B4513";
    palette.stateFull = "#5A5A5A";

    // Deshabilitado
    palette.disabledBackground = "#3A3A3A";
    palette.disabledText = "#5A5A5A";

    return palette;
}

ColorPalette ThemeManager::createOceanBluePalette() {
    ColorPalette palette;

    // Colores primarios - Moderno
    palette.primaryColor = "#1E88E5";
    palette.primaryHover = "#1565C0";
    palette.secondaryColor = "#00ACC1";
    palette.secondaryHover = "#00838F";
    palette.actionColor = "#FFA726";
    palette.actionHover = "#FB8C00";
    palette.dangerColor = "#E53935";
    palette.dangerHover = "#C62828";

    // Fondos
    palette.cardBackground = "#FFFFFF";
    palette.cardBackgroundHover = "#F5F5F5";
    palette.borderColor = "#E0E0E0";
    palette.borderSelected = "#1E88E5";

    // Textos
    palette.textPrimary = "#212121";
    palette.textSecondary = "#757575";
    palette.textLight = "#FFFFFF";

    // Estados
    palette.statusSuccess = "#00ACC1";
    palette.statusWarning = "#FFA726";
    palette.statusError = "#E53935";

    // Barras
    palette.progressBackground = "#E0E0E0";
    palette.progressFill = "#1E88E5";

    // Stats
    palette.statSpeed = "#E53935";
    palette.statAcceleration = "#FFA726";
    palette.statHealth = "#00ACC1";
    palette.statMass = "#1E88E5";
    palette.statControl = "#00ACC1";

    // Estados partida
    palette.stateWaiting = "#00ACC1";
    palette.statePlaying = "#FFA726";
    palette.stateFull = "#9E9E9E";

    // Deshabilitado
    palette.disabledBackground = "#E0E0E0";
    palette.disabledText = "#9E9E9E";

    return palette;
}

ColorPalette ThemeManager::createPostApocalypsePalette() {
    ColorPalette palette;

    // Colores primarios - Post-apocalíptico
    palette.primaryColor = "#248030";
    palette.primaryHover = "#136317";
    palette.secondaryColor = "#CE4B23";
    palette.secondaryHover = "#A03A1A";
    palette.actionColor = "#D8963F";
    palette.actionHover = "#B57A2F";
    palette.dangerColor = "#8B0000";
    palette.dangerHover = "#660000";

    // Fondos
    palette.cardBackground = "#E2A35A";
    palette.cardBackgroundHover = "#D8963F";
    palette.borderColor = "#8B6914";
    palette.borderSelected = "#248030";

    // Textos
    palette.textPrimary = "#000000";
    palette.textSecondary = "#4A4A4A";
    palette.textLight = "#FFFFFF";

    // Estados
    palette.statusSuccess = "#248030";
    palette.statusWarning = "#D8963F";
    palette.statusError = "#CE4B23";

    // Barras
    palette.progressBackground = "#D8963F";
    palette.progressFill = "#248030";

    // Stats
    palette.statSpeed = "#CE4B23";
    palette.statAcceleration = "#D8963F";
    palette.statHealth = "#248030";
    palette.statMass = "#8B6914";
    palette.statControl = "#248030";

    // Estados partida
    palette.stateWaiting = "#248030";
    palette.statePlaying = "#D8963F";
    palette.stateFull = "#8B6914";

    // Deshabilitado
    palette.disabledBackground = "#C4945A";
    palette.disabledText = "#6A6A6A";

    return palette;
}
