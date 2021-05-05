
function love.conf(t)
    t.title = "STC - " .. _VERSION -- The title of the window the game is in (string)
    t.author = "Laurens Rodriguez" -- The author of the game (string)
    t.identity = "stc"          -- The name of the save directory (string)
    t.version = "11.3.0"        -- The L0VE version this game was made for (number)
    t.console = false           -- Attach a console (boolean, Windows only)
    t.window.width = 480        -- The window width (number)
    t.window.height = 272       -- The window height (number)
    t.window.fullscreen = false -- Enable fullscreen (boolean)
    t.window.vsync = 1          -- Vertical sync mode (number)
    t.window.msaa = 0           -- The number of samples to use with multi-sampled antialiasing (number)
    t.modules.joystick = false  -- Enable the joystick module (boolean)
    t.modules.audio = true      -- Enable the audio module (boolean)
    t.modules.keyboard = true   -- Enable the keyboard module (boolean)
    t.modules.event = true      -- Enable the event module (boolean)
    t.modules.image = true      -- Enable the image module (boolean)
    t.modules.graphics = true   -- Enable the graphics module (boolean)
    t.modules.timer = true      -- Enable the timer module (boolean)
    t.modules.mouse = false     -- Enable the mouse module (boolean)
    t.modules.sound = true      -- Enable the sound module (boolean)
    t.modules.physics = false   -- Enable the physics module (boolean)
end
