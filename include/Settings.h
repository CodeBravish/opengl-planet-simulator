#pragma once

class Settings {
   public:
    static Settings& get() {
        static Settings instance;
        return instance;
    }

    bool showOrbit = true;
    bool showGravityWell = true;

   private:
    Settings() {}  // private constructor
};  // namespace Settinsclass Settings