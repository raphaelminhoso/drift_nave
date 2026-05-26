#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include <mutex>

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;
		void audioOut(ofSoundBuffer& buffer) override;

	private:
		struct Meteor {
			ofVec3f position;
			ofVec3f drift;
			ofColor color;
			float radius = 40.0f;
			float speed = 220.0f;
			float acceleration = 0.0f;
			float maxSpeed = 520.0f;
			float surgeStrength = 0.0f;
			float surgeFrequency = 1.0f;
			float motionPhase = 0.0f;
			float rotation = 0.0f;
			float rotationSpeed = 45.0f;
			float dwell = 0.0f;
			float explosion = 0.0f;
			int hitPoints = 1;
			int maxHitPoints = 1;
			bool heavy = false;
			bool exploding = false;
		};

		struct Missile {
			ofVec2f start;
			ofVec2f end;
			ofColor color;
			float age = 0.0f;
			float duration = 0.38f;
		};

		struct Particle {
			ofVec3f position;
			ofVec3f velocity;
			ofColor color;
			float age = 0.0f;
			float lifetime = 0.75f;
			float size = 5.0f;
		};

		struct BossOrb {
			ofVec3f position;
			ofVec3f velocity;
			ofColor color;
			float radius = 24.0f;
			float dwell = 0.0f;
			float explosion = 0.0f;
			float phase = 0.0f;
			bool exploding = false;
		};

		struct PowerUp {
			ofVec3f position;
			ofVec3f drift;
			ofColor color;
			int type = 0;
			float radius = 32.0f;
			float dwell = 0.0f;
			float phase = 0.0f;
			float speed = 140.0f;
			bool collected = false;
		};

		struct BossTarget {
			ofVec2f position;
			float age = 0.0f;
			float duration = 2.8f;
			float radius = 54.0f;
			float phase = 0.0f;
			bool resolved = false;
		};

		struct Star {
			ofVec2f position;
			float depth = 0.0f;
			float size = 1.0f;
			float twinkle = 0.0f;
			ofColor color;
		};

		struct TunnelParticle {
			float angle = 0.0f;
			float radius = 0.0f;
			float speed = 0.0f;
			float size = 1.0f;
			float offset = 0.0f;
			ofColor color;
		};

		struct SynthVoice {
			int type = 0;
			float age = 0.0f;
			float duration = 0.25f;
			float freqStart = 220.0f;
			float freqEnd = 80.0f;
			float volume = 0.2f;
			float pan = 0.5f;
			float phase = 0.0f;
			unsigned int noiseSeed = 1;
		};

		void resetMeteor(Meteor& meteor, bool startFar);
		void resetGame();
		void drawBackground();
		void drawCoverImage(const ofImage& image);
		void drawStartMenu();
		void drawTutorialScreen();
		void startGame();
		ofRectangle startButtonRect() const;
		ofRectangle tutorialButtonRect() const;
		bool isTutorialButtonHit(int x, int y) const;
		void drawProceduralSpaceBackground();
		void drawNebulaRibbons();
		void drawHorizonGlow();
		void drawSynthwaveGrid();
		void drawParticleTunnel();
		void drawPortalOverlay();
		void drawMeteor(const Meteor& meteor);
		void drawBoss();
		void drawBossOrbs();
		void drawBossTargets();
		void drawPowerUps();
		void drawPowerUpIndicators();
		void drawShip();
		void drawParticles();
		void drawMissiles();
		void drawCrosshair();
		void drawHud();
		void drawBossHud();
		void drawBossWarningOverlay();
		void drawBossDeathGlitchOverlay();
		void updateParticles(float dt);
		void updateBoss(float dt);
		void updateBossTargets(float dt);
		void updatePowerUps(float dt);
		void updateAimAssist(float dt);
		float currentDwellProgress() const;
		float hitRadiusFor(const Meteor& meteor) const;
		bool isMouseOver(const Meteor& meteor) const;
		ofVec2f screenPositionFor(const Meteor& meteor) const;
		ofVec2f bossScreenPosition() const;
		ofVec2f shipScreenPosition() const;
		void launchMissileAt(const Meteor& meteor);
		void launchMissileAt(const ofVec2f& target, const ofColor& color);
		void spawnExplosionParticles(const Meteor& meteor, int amount);
		void spawnExplosionParticles(const ofVec3f& position, const ofColor& color, int amount);
		void startBossFight();
		void startBossWarning();
		void createBossCyberpunkTexture();
		void spawnBossOrb();
		void spawnBossTarget();
		void spawnPowerUp(bool startFar = true);
		void collectPowerUp(PowerUp& powerUp);
		void damageBoss();
		void activatePowerShot();
		void explodeMeteor(Meteor& meteor);
		void spawnInitialMeteors();
		void startNextWave();
		void triggerSound(int type, float intensity = 1.0f, float pan = 0.5f);

		ofVideoPlayer backgroundVideo;
		ofImage introImage;
		ofImage tutorialImage;
		ofImage tutorialButtonImage;
		ofImage victoryImage;
		ofImage backgroundStill;
		ofImage bossProceduralTexture;
		vector<ofImage> backgroundFrames;
		ofImage crosshairImage;
		ofxAssimpModelLoader meteorModel;
		ofxAssimpModelLoader shipModel;
		ofxAssimpModelLoader bossModel;
		ofCamera camera;
		ofLight keyLight;
		ofSoundStream soundStream;
		ofSoundPlayer backgroundMusic;
		ofTrueTypeFont startButtonFont;
		ofTrueTypeFont warningTitleFont;
		ofTrueTypeFont warningSubtitleFont;
		ofTrueTypeFont tutorialHeaderFont;
		ofTrueTypeFont tutorialBodyFont;
		vector<Meteor> meteors;
		vector<BossOrb> bossOrbs;
		vector<BossTarget> bossTargets;
		vector<PowerUp> powerUps;
		vector<Missile> missiles;
		vector<Particle> particles;
		vector<Star> stars;
		vector<TunnelParticle> tunnelParticles;
		ofVec2f mousePosition;
		ofVec2f aimPosition;
		ofVec2f assistedTarget;

		int score = 0;
		int combo = 0;
		int maxLives = 12;
		int lives = 12;
		int impacts = 0;
		int wave = 1;
		int destroyedInWave = 0;
		int meteorsPerWave = 4;
		int activeMeteorCount = 8;
		float dwellToExplode = 0.45f;
		float nearZ = 720.0f;
		float farZ = -1900.0f;
		float speedMultiplier = 1.08f;
		float gameTimer = 0.0f;
		float targetGameplaySeconds = 30.0f;
		float aimAssistRadius = 250.0f;
		float aimAssistStrength = 0.58f;
		float damageFlash = 0.0f;
		float screenShake = 0.0f;
		float waveMessageTimer = 1.8f;
		float powerCharge = 0.0f;
		float powerPulse = 0.0f;
		float bossTimer = 0.0f;
		float bossDwell = 0.0f;
		float bossOrbCooldown = 0.0f;
		float bossWarningTimer = 0.0f;
		float bossWarningSoundTimer = 0.0f;
		float bossIntroTimer = 0.0f;
		float bossDeathGlitchTimer = 0.0f;
		float victoryScreenTimer = 0.0f;
		float bossRadius = 165.0f;
		float bossModelScale = 81.0f;
		float bossModelRotationX = -255.0f;
		float bossModelRotationY = 0.0f;
		float bossModelRotationZ = 180.0f;
		float powerUpSpawnTimer = 2.5f;
		float shieldTimer = 0.0f;
		float speedBoostTimer = 0.0f;
		float heavyShotTimer = 0.0f;
		float powerUpNoticeTimer = 0.0f;
		float backgroundFrameRate = 8.0f;
		int audioSampleRate = 44100;
		int bossHealth = 24;
		int maxBossHealth = 24;
		int bossOrbsDestroyedThisPhase = 0;
		int bossOrbsToOpen = 8;
		float bossTargetCooldown = 2.2f;
		string lastPowerUpLabel;
		float humPhase = 0.0f;
		float humPhase2 = 0.0f;
		vector<SynthVoice> synthVoices;
		std::mutex audioMutex;
		bool useProceduralBackground = true;
		bool gameStarted = false;
		bool gameWon = false;
		bool tutorialActive = false;
		bool bossActive = false;
		bool bossWarningActive = false;
		bool bossVulnerable = false;
		bool paused = false;
		bool modelLoaded = false;
		bool meteorUsesRawModel = false;
		bool shipModelLoaded = false;
		bool bossModelLoaded = false;
		bool bossProceduralTextureReady = false;
		bool backgroundMusicLoaded = false;
		bool startButtonFontLoaded = false;
		bool warningTitleFontLoaded = false;
		bool warningSubtitleFontLoaded = false;
		bool tutorialHeaderFontLoaded = false;
		bool tutorialBodyFontLoaded = false;
		bool hasAssistedTarget = false;
		ofVec3f bossWorldPosition() const;
};
