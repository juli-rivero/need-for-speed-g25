#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QString>
#include <QObject>

/**
 * Enum para identificar los temas disponibles
 */
enum class ThemeType {
    Default,        // 🎨 Material Design (Verde/Azul)
    DarkMode,       // 🌙 Cyberpunk (Púrpura/Cyan)
    RacingRed,      // 🔥 Ferrari (Rojo/Amarillo)
    Luxury,         // 💎 Premium (Dorado/Azul oscuro)
    OceanBlue,      // 🌊 Moderno (Azul/Cyan)
    PostApocalypse  // ☢️ Post-apocalíptico (Verde militar/Naranja)
};

/**
 * Estructura que define una paleta de colores completa
 */
struct ColorPalette {
    // Colores primarios
    QString primaryColor;
    QString primaryHover;
    QString secondaryColor;
    QString secondaryHover;
    QString actionColor;
    QString actionHover;
    QString dangerColor;
    QString dangerHover;
    
    // Fondos
    QString cardBackground;
    QString cardBackgroundHover;
    QString borderColor;
    QString borderSelected;
    
    // Textos
    QString textPrimary;
    QString textSecondary;
    QString textLight;
    
    // Estados
    QString statusSuccess;
    QString statusWarning;
    QString statusError;
    
    // Barras de progreso
    QString progressBackground;
    QString progressFill;
    
    // Stats de autos
    QString statSpeed;
    QString statAcceleration;
    QString statHealth;
    QString statMass;
    QString statControl;
    
    // Estados de partida
    QString stateWaiting;
    QString statePlaying;
    QString stateFull;
    
    // Deshabilitado
    QString disabledBackground;
    QString disabledText;
};

/**
 * Manager singleton para gestionar temas del Lobby
 */
class ThemeManager : public QObject {
    Q_OBJECT

public:
    static ThemeManager& instance() {
        static ThemeManager inst;
        return inst;
    }
    
    // Obtener paleta actual
    const ColorPalette& getCurrentPalette() const { return currentPalette; }
    
    // Cambiar tema
    void setTheme(ThemeType type);
    
    // Obtener tipo de tema actual
    ThemeType getCurrentTheme() const { return currentTheme; }
    
    // Obtener nombre del tema
    QString getThemeName(ThemeType type) const;
    
    // Constantes de estilo
    static constexpr int BORDER_RADIUS = 8;
    static constexpr int CARD_BORDER_RADIUS = 8;
    static constexpr int FONT_SIZE_LARGE = 20;
    static constexpr int FONT_SIZE_MEDIUM = 16;
    static constexpr int FONT_SIZE_NORMAL = 14;
    static constexpr int FONT_SIZE_SMALL = 12;
    static constexpr int FONT_SIZE_TINY = 9;
    
    // Funciones de estilo
    QString buttonPrimaryStyle() const;
    QString buttonSecondaryStyle() const;
    QString buttonActionStyle() const;
    QString buttonDangerStyle() const;
    QString gameCardStyle(bool selected = false) const;
    QString carCardStyle() const;  // Estilo para tarjetas de selección de autos
    QString progressBarStyle(const QString& fillColor = "") const;
    QString carStatBarStyle(float value) const;  // Estilo para barras de stats con color según valor
    QString statusSuccessStyle() const;
    QString statusWarningStyle() const;
    QString statusErrorStyle() const;

signals:
    void themeChanged();

private:
    ThemeManager();
    ~ThemeManager() = default;
    
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;
    
    ColorPalette createDefaultPalette();
    ColorPalette createDarkModePalette();
    ColorPalette createRacingRedPalette();
    ColorPalette createLuxuryPalette();
    ColorPalette createOceanBluePalette();
    ColorPalette createPostApocalypsePalette();
    
    ThemeType currentTheme;
    ColorPalette currentPalette;
};

// Alias para acceso rápido
inline ThemeManager& Theme() {
    return ThemeManager::instance();
}

#endif // THEME_MANAGER_H
