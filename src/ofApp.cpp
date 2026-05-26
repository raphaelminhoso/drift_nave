#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableDepthTest();
	ofEnableAlphaBlending();
	ofSetCircleResolution(64);
	ofBackground(0);

	ofSoundStreamSettings audioSettings;
	auto audioDevices = soundStream.getMatchingDevices("default");
	if (!audioDevices.empty()) {
		audioSettings.setOutDevice(audioDevices[0]);
	}
	audioSettings.setOutListener(this);
	audioSettings.sampleRate = audioSampleRate;
	audioSettings.numOutputChannels = 2;
	audioSettings.numInputChannels = 0;
	audioSettings.bufferSize = 512;
	soundStream.setup(audioSettings);

	backgroundMusicLoaded = backgroundMusic.load("Feel The Steel Synthwave.mp3");
	if (backgroundMusicLoaded) {
		backgroundMusic.setLoop(true);
		backgroundMusic.setMultiPlay(false);
		backgroundMusic.setVolume(0.45f);
	}
	startButtonFontLoaded = startButtonFont.load("C:/Windows/Fonts/OCRAEXT.TTF", 22, true, true);
	if (!startButtonFontLoaded) {
		startButtonFontLoaded = startButtonFont.load("C:/Windows/Fonts/consolab.ttf", 22, true, true);
	}
	if (!startButtonFontLoaded) {
		startButtonFontLoaded = startButtonFont.load("C:/Windows/Fonts/arialbd.ttf", 22, true, true);
	}
	warningTitleFontLoaded = warningTitleFont.load("C:/Windows/Fonts/impact.ttf", 128, true, true);
	if (!warningTitleFontLoaded) {
		warningTitleFontLoaded = warningTitleFont.load("C:/Windows/Fonts/arialbd.ttf", 128, true, true);
	}
	warningSubtitleFontLoaded = warningSubtitleFont.load("C:/Windows/Fonts/OCRAEXT.TTF", 54, true, true);
	if (!warningSubtitleFontLoaded) {
		warningSubtitleFontLoaded = warningSubtitleFont.load("C:/Windows/Fonts/arialbd.ttf", 54, true, true);
	}
	tutorialHeaderFontLoaded = tutorialHeaderFont.load("C:/Windows/Fonts/OCRAEXT.TTF", 20, true, true);
	if (!tutorialHeaderFontLoaded) {
		tutorialHeaderFontLoaded = tutorialHeaderFont.load("C:/Windows/Fonts/consolab.ttf", 20, true, true);
	}
	tutorialBodyFontLoaded = tutorialBodyFont.load("C:/Windows/Fonts/consola.ttf", 16, true, true);
	if (!tutorialBodyFontLoaded) {
		tutorialBodyFontLoaded = tutorialBodyFont.load("C:/Windows/Fonts/arial.ttf", 16, true, true);
	}

	mousePosition.set(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);
	aimPosition = mousePosition;

	introImage.load("BG_tela_into.png");
	tutorialImage.load("TELA_tutorial.jpg");
	tutorialButtonImage.load("botao_tutorial.png");
	victoryImage.load("telavitoria.png");
	backgroundStill.load("custom_bg.png");
	if (!backgroundStill.isAllocated()) {
		backgroundStill.load("Video_BG.jpg");
	}
	ofDirectory bgDir("bg_frames");
	bgDir.allowExt("jpg");
	bgDir.listDir();
	bgDir.sort();
	backgroundFrames.reserve(bgDir.size());
	for (const auto& file : bgDir.getFiles()) {
		ofImage frame;
		if (frame.load(file.getAbsolutePath())) {
			backgroundFrames.push_back(std::move(frame));
		}
	}

	crosshairImage.load("mira_crosshair.png");
	meteorUsesRawModel = meteorModel.load("meteoro_001.obj");
	modelLoaded = meteorUsesRawModel;
	if (!modelLoaded) {
		meteorUsesRawModel = meteorModel.load("metoro.fbx");
		modelLoaded = meteorUsesRawModel;
	}
	if (!modelLoaded) {
		meteorUsesRawModel = false;
		modelLoaded = meteorModel.load("Meteor_mp.obj");
	}
	if (modelLoaded) {
		meteorModel.setScaleNormalization(!meteorUsesRawModel);
		if (!meteorUsesRawModel) {
			meteorModel.setNormalizationFactor(1.0f);
		}
		meteorModel.disableMaterials();
		if (meteorUsesRawModel) {
			meteorModel.enableTextures();
		} else {
			meteorModel.disableTextures();
		}
		meteorModel.disableCulling();
	}
	shipModelLoaded = shipModel.load("nave.obj");
	if (shipModelLoaded) {
		shipModel.setScaleNormalization(false);
		shipModel.enableMaterials();
		shipModel.enableTextures();
		shipModel.disableCulling();
	}
	bossModelLoaded = bossModel.load("nave_boss.obj");
	if (bossModelLoaded) {
		bossModel.setScaleNormalization(false);
		bossModel.disableMaterials();
		bossModel.disableTextures();
		bossModel.disableCulling();
	}
	createBossCyberpunkTexture();

	camera.setPosition(0, 0, 900);
	camera.lookAt(glm::vec3(0, 0, 0));
	camera.setFov(62.0f);
	camera.setNearClip(1.0f);
	camera.setFarClip(5000.0f);

	keyLight.setup();
	keyLight.setPointLight();
	keyLight.setPosition(-240, 320, 520);
	keyLight.setDiffuseColor(ofFloatColor(1.0f, 0.82f, 0.62f));
	keyLight.setSpecularColor(ofFloatColor(1.0f));
	ofSetGlobalAmbientColor(ofColor(135, 145, 175));

	stars.clear();
	for (int i = 0; i < 340; ++i) {
		Star star;
		star.position.set(ofRandomWidth(), ofRandomHeight());
		star.depth = ofRandom(0.15f, 1.0f);
		star.size = ofRandom(0.55f, 2.6f);
		star.twinkle = ofRandom(TWO_PI);
		star.color = ofColor::fromHsb(ofRandom(135, 198), ofRandom(25, 120), ofRandom(170, 255));
		stars.push_back(star);
	}

	tunnelParticles.clear();
	for (int i = 0; i < 210; ++i) {
		TunnelParticle particle;
		particle.angle = ofRandom(TWO_PI);
		particle.radius = ofRandom(0.12f, 1.0f);
		particle.speed = ofRandom(0.20f, 0.68f);
		particle.size = ofRandom(1.0f, 3.8f);
		particle.offset = ofRandom(1.0f);
		particle.color = ofColor::fromHsb((int)ofRandom(132, 210), ofRandom(105, 220), ofRandom(190, 255));
		tunnelParticles.push_back(particle);
	}

	spawnInitialMeteors();
}

void ofApp::createBossCyberpunkTexture() {
	const int texSize = 512;
	ofPixels pixels;
	pixels.allocate(texSize, texSize, OF_PIXELS_RGB);

	for (int y = 0; y < texSize; ++y) {
		for (int x = 0; x < texSize; ++x) {
			const float nx = x / static_cast<float>(texSize);
			const float ny = y / static_cast<float>(texSize);
			const float scan = 0.5f + 0.5f * sinf(ny * 96.0f);
			const float panel = ((x / 64 + y / 96) % 2 == 0) ? 1.0f : 0.72f;
			ofColor base(
				10 + 10 * panel + scan * 7,
				14 + 18 * panel + scan * 12,
				28 + 34 * panel + scan * 18
			);

			const bool fineGrid = (x % 32 == 0) || (y % 32 == 0);
			const bool majorGrid = (x % 128 < 3) || (y % 128 < 3);
			const bool diagonalA = abs(((x + y * 2) % 118) - 59) < 2;
			const bool diagonalB = abs(((x * 2 - y + texSize * 4) % 151) - 75) < 2;
			const bool circuitNode = (x % 128 < 10 && y % 96 < 10) || (x % 96 > 82 && y % 128 > 114);

			if (fineGrid) {
				base = base.getLerped(ofColor(35, 215, 255), 0.18f);
			}
			if (majorGrid || diagonalA) {
				base = base.getLerped(ofColor(55, 245, 255), 0.72f);
			}
			if (diagonalB) {
				base = base.getLerped(ofColor(255, 45, 220), 0.66f);
			}
			if (circuitNode) {
				base = base.getLerped(ofColor(255, 225, 90), 0.78f);
			}

			const float vignette = ofClamp(1.18f - ofDist(nx, ny, 0.5f, 0.5f) * 0.85f, 0.42f, 1.0f);
			base.r *= vignette;
			base.g *= vignette;
			base.b *= vignette;
			pixels.setColor(x, y, base);
		}
	}

	const bool wasUsingArbTex = ofGetUsingArbTex();
	ofDisableArbTex();
	bossProceduralTexture.setFromPixels(pixels);
	if (wasUsingArbTex) {
		ofEnableArbTex();
	}

	if (bossProceduralTexture.isAllocated()) {
		bossProceduralTexture.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
		bossProceduralTexture.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
		bossProceduralTextureReady = true;
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	const float dt = ofGetLastFrameTime();
	if (backgroundVideo.isLoaded()) {
		backgroundVideo.update();
	}

	for (auto& missile : missiles) {
		missile.age += dt;
	}
	missiles.erase(
		remove_if(missiles.begin(), missiles.end(), [](const Missile& missile) {
			return missile.age >= missile.duration;
		}),
		missiles.end());
	updateParticles(dt);
	powerPulse = ofClamp(powerPulse - dt * 1.6f, 0.0f, 1.0f);

	if (backgroundMusicLoaded) {
		if (gameStarted && !backgroundMusic.isPlaying()) {
			backgroundMusic.play();
		} else if (!gameStarted && backgroundMusic.isPlaying()) {
			backgroundMusic.stop();
		}
	}

	if (!gameStarted) {
		if (gameWon && victoryScreenTimer > 0.0f) {
			victoryScreenTimer = std::max(0.0f, victoryScreenTimer - dt);
			if (victoryScreenTimer <= 0.0f) {
				gameWon = false;
			}
		}
		return;
	}

	if (bossDeathGlitchTimer > 0.0f) {
		bossDeathGlitchTimer = std::max(0.0f, bossDeathGlitchTimer - dt);
		screenShake = std::max(screenShake, 1.0f);
		damageFlash = std::max(damageFlash, 0.35f);
		if (bossDeathGlitchTimer <= 0.0f) {
			gameWon = true;
			victoryScreenTimer = 3.0f;
			gameStarted = false;
			damageFlash = 0.0f;
			screenShake = 0.0f;
		}
		return;
	}

	if (paused) {
		return;
	}

	gameTimer += dt;
	damageFlash = ofClamp(damageFlash - dt * 2.8f, 0.0f, 1.0f);
	screenShake = ofClamp(screenShake - dt * 3.2f, 0.0f, 1.0f);
	shieldTimer = ofClamp(shieldTimer - dt, 0.0f, 20.0f);
	speedBoostTimer = ofClamp(speedBoostTimer - dt, 0.0f, 20.0f);
	heavyShotTimer = ofClamp(heavyShotTimer - dt, 0.0f, 20.0f);
	powerUpNoticeTimer = ofClamp(powerUpNoticeTimer - dt, 0.0f, 2.2f);
	bossIntroTimer = ofClamp(bossIntroTimer - dt, 0.0f, 2.2f);

	if (bossWarningActive) {
		bossWarningTimer -= dt;
		bossWarningSoundTimer -= dt;
		screenShake = std::max(screenShake, 0.2f + 0.35f * ofClamp(bossWarningTimer / 2.8f, 0.0f, 1.0f));
		if (bossWarningSoundTimer <= 0.0f) {
			triggerSound(4, 1.25f, 0.5f);
			bossWarningSoundTimer = 0.34f;
		}
		if (bossWarningTimer <= 0.0f) {
			startBossFight();
		}
		return;
	}

	updateAimAssist(dt);
	updatePowerUps(dt);

	if (!bossActive && gameTimer >= targetGameplaySeconds) {
		startBossWarning();
	}

	if (bossActive) {
		updateBoss(dt);
		return;
	}

	for (auto& meteor : meteors) {
		const float activeDwellToExplode = speedBoostTimer > 0.0f ? dwellToExplode * 0.58f : dwellToExplode;
		if (meteor.exploding) {
			meteor.explosion += dt;
			if (meteor.explosion > 0.42f) {
				resetMeteor(meteor, true);
			}
			continue;
		}

		const float depth01 = ofMap(meteor.position.z, farZ, nearZ, 0.0f, 1.0f, true);
		meteor.speed = std::min(meteor.maxSpeed, meteor.speed + meteor.acceleration * ofLerp(0.75f, 2.0f, depth01) * dt);
		const float speedBoost = ofLerp(0.78f, 2.25f, depth01 * depth01);
		const float surgeWave = sinf(ofGetElapsedTimef() * meteor.surgeFrequency + meteor.motionPhase);
		const float surge = 1.0f + std::max(0.0f, surgeWave) * meteor.surgeStrength;
		const float hoverSlowdown = isMouseOver(meteor) ? 0.52f : 1.0f;
		const float currentSpeed = meteor.speed * speedBoost * surge * speedMultiplier * hoverSlowdown;
		const float lateralRush = ofLerp(0.4f, 1.55f, depth01);
		meteor.position.z += currentSpeed * dt;
		meteor.position.x += (meteor.drift.x + sin(ofGetElapsedTimef() * 2.4f + meteor.motionPhase) * 18.0f * lateralRush) * dt;
		meteor.position.y += (meteor.drift.y + cos(ofGetElapsedTimef() * 1.9f + meteor.motionPhase) * 10.0f * lateralRush) * dt;
		meteor.rotation += meteor.rotationSpeed * dt;

		if (isMouseOver(meteor)) {
			meteor.dwell += dt;
			if (meteor.dwell >= activeDwellToExplode) {
				explodeMeteor(meteor);
			}
		} else {
			meteor.dwell = ofClamp(meteor.dwell - dt * 1.8f, 0.0f, activeDwellToExplode);
		}

		if (meteor.position.z > nearZ) {
			if (shieldTimer > 0.0f) {
				shieldTimer = std::max(0.0f, shieldTimer - 2.2f);
			} else {
				lives = std::max(0, lives - 1);
			}
			combo = 0;
			damageFlash = 1.0f;
			screenShake = 1.0f;
			triggerSound(2, 1.0f, ofMap(screenPositionFor(meteor).x, 0, ofGetWidth(), 0.1f, 0.9f, true));
			resetMeteor(meteor, true);
			if (lives <= 0) {
				gameStarted = false;
				paused = false;
				bossWarningActive = false;
				damageFlash = 0.0f;
				screenShake = 0.0f;
				powerPulse = 0.0f;
				shieldTimer = 0.0f;
				speedBoostTimer = 0.0f;
				heavyShotTimer = 0.0f;
				missiles.clear();
				powerUps.clear();
				triggerSound(3, 1.35f, 0.5f);
				break;
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0, 0, 0, 255);
	ofDisableLighting();
	ofDisableBlendMode();
	ofEnableAlphaBlending();
	ofSetColor(255);
	ofFill();
	ofSetLineWidth(1.0f);
	ofDisableDepthTest();
	if (!gameStarted) {
		if (tutorialActive) {
			drawTutorialScreen();
			return;
		}
		drawStartMenu();
		return;
	}

	drawBackground();

	ofVec2f shakeOffset(0.0f, 0.0f);
	if (screenShake > 0.0f) {
		const float amount = 28.0f * screenShake;
		shakeOffset.set(ofRandom(-amount, amount), ofRandom(-amount, amount));
	}

	ofEnableDepthTest();
	ofEnableLighting();
	keyLight.enable();
	camera.begin();
	ofTranslate(shakeOffset.x, shakeOffset.y, 0);
	for (const auto& meteor : meteors) {
		drawMeteor(meteor);
	}
	drawBoss();
	drawBossOrbs();
	drawPowerUps();
	drawParticles();
	camera.end();
	keyLight.disable();
	ofDisableLighting();

	ofDisableDepthTest();
	drawMissiles();

	ofEnableDepthTest();
	ofEnableLighting();
	keyLight.enable();
	camera.begin();
	ofTranslate(shakeOffset.x, shakeOffset.y, 0);
	drawShip();
	camera.end();
	keyLight.disable();
	ofDisableLighting();

	ofDisableDepthTest();
	if (bossDeathGlitchTimer > 0.0f) {
		drawBossDeathGlitchOverlay();
		return;
	}

	if (damageFlash > 0.0f) {
		ofSetColor(255, 20, 12, 120 * damageFlash);
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	}
	drawPowerUpIndicators();
	drawBossTargets();
	drawCrosshair();
	drawHud();
	drawBossWarningOverlay();
}

//--------------------------------------------------------------
void ofApp::exit(){
	if (backgroundMusicLoaded) {
		backgroundMusic.stop();
	}
	soundStream.close();
	backgroundVideo.stop();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (!gameStarted && gameWon && victoryScreenTimer > 0.0f) {
		return;
	}

	if (tutorialActive) {
		if (key == OF_KEY_RETURN || key == ' ') {
			startGame();
		} else if (key == OF_KEY_ESC) {
			tutorialActive = false;
		}
		return;
	}

	if (key == 'b' || key == 'B') {
		tutorialActive = false;
		resetGame();
		gameStarted = true;
		startBossFight();
		bossVulnerable = true;
		return;
	}

	if (key == '1') {
		bossModelRotationY -= 15.0f;
		return;
	}
	if (key == '2') {
		bossModelRotationY += 15.0f;
		return;
	}
	if (key == '3') {
		bossModelRotationZ -= 15.0f;
		return;
	}
	if (key == '4') {
		bossModelRotationZ += 15.0f;
		return;
	}
	if (key == '5') {
		bossModelScale = std::max(6.0f, bossModelScale - 2.0f);
		return;
	}
	if (key == '6') {
		bossModelScale = std::min(180.0f, bossModelScale + 4.0f);
		return;
	}
	if (key == '7') {
		bossModelRotationX -= 15.0f;
		return;
	}
	if (key == '8') {
		bossModelRotationX += 15.0f;
		return;
	}

	if (!gameStarted && (key == OF_KEY_RETURN || key == ' ')) {
		tutorialActive = true;
		triggerSound(3, 0.8f, 0.5f);
		return;
	}

	if (key == 'r' || key == 'R') {
		resetGame();
		gameStarted = true;
	}

	if (key == 'p' || key == 'P') {
		paused = !paused;
	}

	if (key == ' ') {
		activatePowerShot();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	mousePosition.set(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	mousePosition.set(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (!gameStarted && gameWon && victoryScreenTimer > 0.0f) {
		return;
	}

	if (tutorialActive) {
		if (isTutorialButtonHit(x, y)) {
			startGame();
		}
		return;
	}

	if (!gameStarted && startButtonRect().inside(x, y)) {
		tutorialActive = true;
		triggerSound(3, 0.8f, 0.5f);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
}

void ofApp::audioOut(ofSoundBuffer& buffer) {
	std::lock_guard<std::mutex> lock(audioMutex);

	const float twoPi = TWO_PI;
	for (size_t i = 0; i < buffer.getNumFrames(); ++i) {
		humPhase += twoPi * 46.0f / audioSampleRate;
		humPhase2 += twoPi * 72.0f / audioSampleRate;
		if (humPhase > twoPi) humPhase -= twoPi;
		if (humPhase2 > twoPi) humPhase2 -= twoPi;

		float left = (sinf(humPhase) * 0.018f + sinf(humPhase2) * 0.009f);
		float right = (sinf(humPhase + 0.65f) * 0.018f + sinf(humPhase2 + 0.4f) * 0.009f);

		for (auto& voice : synthVoices) {
			const float p = ofClamp(voice.age / voice.duration, 0.0f, 1.0f);
			const float env = (1.0f - p) * (1.0f - p);
			const float freq = ofLerp(voice.freqStart, voice.freqEnd, p);
			voice.phase += twoPi * freq / audioSampleRate;
			if (voice.phase > twoPi) voice.phase -= twoPi;

			voice.noiseSeed = 1664525u * voice.noiseSeed + 1013904223u;
			const float noise = ((voice.noiseSeed >> 8) / (float)0x00FFFFFF) * 2.0f - 1.0f;
			float sample = 0.0f;
		if (voice.type == 0) {
				sample = sinf(voice.phase) * env;
			} else if (voice.type == 1) {
				sample = (sinf(voice.phase) * 0.55f + noise * 0.45f) * env;
			} else if (voice.type == 2) {
				sample = noise * env;
		} else if (voice.type == 4) {
			const float alarmGate = sinf(voice.age * 95.0f) > -0.1f ? 1.0f : 0.25f;
			sample = (sinf(voice.phase) * 0.78f + sinf(voice.phase * 0.5f) * 0.22f) * env * alarmGate;
			} else {
				sample = (sinf(voice.phase) + sinf(voice.phase * 0.51f)) * 0.5f * env;
			}

			left += sample * voice.volume * (1.0f - voice.pan);
			right += sample * voice.volume * voice.pan;
			voice.age += 1.0f / audioSampleRate;
		}

		buffer[i * buffer.getNumChannels()] = ofClamp(left, -0.85f, 0.85f);
		buffer[i * buffer.getNumChannels() + 1] = ofClamp(right, -0.85f, 0.85f);
	}

	synthVoices.erase(
		remove_if(synthVoices.begin(), synthVoices.end(), [](const SynthVoice& voice) {
			return voice.age >= voice.duration;
		}),
		synthVoices.end());
}

void ofApp::spawnInitialMeteors() {
	meteors.assign(activeMeteorCount, Meteor());
	for (auto& meteor : meteors) {
		resetMeteor(meteor, true);
	}
}

void ofApp::resetGame() {
	score = 0;
	combo = 0;
	lives = maxLives;
	impacts = 0;
	wave = 1;
	destroyedInWave = 0;
	meteorsPerWave = 4;
	speedMultiplier = 1.0f;
	gameTimer = 0.0f;
	targetGameplaySeconds = 30.0f;
	damageFlash = 0.0f;
	screenShake = 0.0f;
	waveMessageTimer = 1.8f;
	powerCharge = 0.0f;
	powerPulse = 0.0f;
	bossActive = false;
	bossWarningActive = false;
	bossVulnerable = false;
	gameWon = false;
	bossHealth = maxBossHealth;
	bossOrbsDestroyedThisPhase = 0;
	bossTimer = 0.0f;
	bossDwell = 0.0f;
	bossOrbCooldown = 0.0f;
	bossWarningTimer = 0.0f;
	bossWarningSoundTimer = 0.0f;
	bossIntroTimer = 0.0f;
	bossDeathGlitchTimer = 0.0f;
	victoryScreenTimer = 0.0f;
	bossTargetCooldown = 2.2f;
	powerUpSpawnTimer = 2.4f;
	shieldTimer = 0.0f;
	speedBoostTimer = 0.0f;
	heavyShotTimer = 0.0f;
	powerUpNoticeTimer = 0.0f;
	lastPowerUpLabel = "";
	tutorialActive = false;
	bossOrbs.clear();
	bossTargets.clear();
	powerUps.clear();
	missiles.clear();
	particles.clear();
	paused = false;
	spawnInitialMeteors();
}

void ofApp::resetMeteor(Meteor& meteor, bool startFar) {
	meteor.exploding = false;
	meteor.explosion = 0.0f;
	meteor.dwell = 0.0f;

	const float z = startFar ? ofRandom(farZ, -1250.0f) : ofRandom(farZ, nearZ);
	const float depth01 = ofMap(z, farZ, nearZ, 0.0f, 1.0f, true);
	const float spreadX = ofLerp(170.0f, 720.0f, depth01);
	const float spreadY = ofLerp(150.0f, 520.0f, depth01);

	meteor.heavy = ofRandom(1.0f) < 0.22f;
	meteor.maxHitPoints = meteor.heavy ? (int)ofRandom(3, 6) : 1;
	meteor.hitPoints = meteor.maxHitPoints;
	meteor.position.set(ofRandom(-spreadX, spreadX), ofRandom(-spreadY, spreadY), z);
	meteor.drift.set(ofRandom(-42.0f, 42.0f), ofRandom(-18.0f, 18.0f), 0);
	meteor.radius = meteor.heavy ? ofRandom(58.0f, 86.0f) : ofRandom(24.0f, 62.0f);
	meteor.speed = meteor.heavy ? ofRandom(82.0f, 138.0f) + wave * 3.2f : ofRandom(205.0f, 340.0f) + wave * 8.0f;
	meteor.acceleration = meteor.heavy ? ofRandom(6.0f, 22.0f) + wave * 1.0f : ofRandom(20.0f, 76.0f) + wave * 2.4f;
	meteor.maxSpeed = meteor.heavy ? ofRandom(190.0f, 315.0f) + wave * 6.5f : ofRandom(455.0f, 710.0f) + wave * 14.0f;
	meteor.surgeStrength = meteor.heavy ? ofRandom(0.05f, 0.22f) : ofRandom(0.22f, 0.86f);
	meteor.surgeFrequency = meteor.heavy ? ofRandom(0.7f, 1.4f) : ofRandom(1.4f, 3.6f);
	meteor.motionPhase = ofRandom(TWO_PI);
	meteor.rotation = ofRandom(360.0f);
	meteor.rotationSpeed = meteor.heavy ? ofRandom(-55.0f, 55.0f) : ofRandom(-170.0f, 170.0f);

	const vector<ofColor> palette = {
		ofColor(255, 140, 90),
		ofColor(255, 78, 52),
		ofColor(220, 100, 145),
		ofColor(180, 80, 255),
		ofColor(255, 195, 100),
		ofColor(255, 95, 32)
	};
	meteor.color = meteor.heavy ? ofColor(255, 54, 32) : palette[(int)ofRandom(palette.size())];
}

void ofApp::drawBackground() {
	ofDisableLighting();
	ofSetColor(255);
	if (useProceduralBackground) {
		drawProceduralSpaceBackground();
	} else if (!backgroundFrames.empty()) {
		const int frameIndex = ((int)(ofGetElapsedTimef() * backgroundFrameRate)) % backgroundFrames.size();
		drawCoverImage(backgroundFrames[frameIndex]);
	} else if (backgroundVideo.isLoaded() && backgroundVideo.getTexture().isAllocated()) {
		const float videoW = backgroundVideo.getWidth();
		const float videoH = backgroundVideo.getHeight();
		const float scale = max(ofGetWidth() / videoW, ofGetHeight() / videoH);
		const float drawW = videoW * scale;
		const float drawH = videoH * scale;
		backgroundVideo.draw((ofGetWidth() - drawW) * 0.5f, (ofGetHeight() - drawH) * 0.5f, drawW, drawH);
	} else if (backgroundStill.isAllocated()) {
		drawCoverImage(backgroundStill);
	} else {
		ofMesh gradient;
		gradient.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
		gradient.addColor(ofColor(20, 2, 34));
		gradient.addVertex(glm::vec3(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f, 0));
		for (int i = 0; i <= 48; ++i) {
			const float a = ofMap(i, 0, 48, 0, TWO_PI);
			gradient.addColor(ofColor(0, 0, 8));
			gradient.addVertex(glm::vec3(ofGetWidth() * 0.5f + cos(a) * ofGetWidth(), ofGetHeight() * 0.5f + sin(a) * ofGetHeight(), 0));
		}
		gradient.draw();
	}

	drawPortalOverlay();
}

void ofApp::drawProceduralSpaceBackground() {
	ofDisableLighting();
	const float w = ofGetWidth();
	const float h = ofGetHeight();
	const ofVec2f center(w * 0.5f, h * 0.52f);
	const float t = ofGetElapsedTimef();

	if (backgroundStill.isAllocated()) {
		drawCoverImage(backgroundStill);
		ofSetColor(18, 5, 42, 24);
		ofDrawRectangle(0, 0, w, h);
	}

	ofMesh gradient;
	gradient.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
	gradient.addColor(ofColor(88, 34, 138, backgroundStill.isAllocated() ? 42 : 255));
	gradient.addVertex(glm::vec3(center.x, center.y, 0));
	const float radius = max(w, h) * 0.95f;
	for (int i = 0; i <= 96; ++i) {
		const float a = ofMap(i, 0, 96, 0, TWO_PI);
		const float hueMix = (sin(a * 2.0f + t * 0.22f) + 1.0f) * 0.5f;
		gradient.addColor(ofColor(
			ofLerp(7, 24, hueMix),
			ofLerp(16, 42, hueMix),
			ofLerp(58, 120, hueMix),
			backgroundStill.isAllocated() ? 48 : 255));
		gradient.addVertex(glm::vec3(center.x + cos(a) * radius, center.y + sin(a) * radius, 0));
	}
	gradient.draw();

	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	drawSynthwaveGrid();

	for (const auto& star : stars) {
		const float speed = ofLerp(6.0f, 42.0f, star.depth);
		float x = fmod(star.position.x + t * speed, w + 40.0f) - 20.0f;
		float y = fmod(star.position.y + sin(t * 0.12f + star.twinkle) * 10.0f, h);
		const float alpha = ofMap(sin(t * (1.4f + star.depth) + star.twinkle), -1.0f, 1.0f, 70.0f, 230.0f);
		ofSetColor(star.color, alpha);
		ofDrawCircle(x, y, star.size * ofLerp(0.7f, 1.8f, star.depth));
	}
	drawParticleTunnel();
	ofDisableBlendMode();
	ofPopStyle();
}

void ofApp::drawNebulaRibbons() {
	const float w = ofGetWidth();
	const float h = ofGetHeight();
	const float t = ofGetElapsedTimef();

	ofPushStyle();
	for (int ribbon = 0; ribbon < 4; ++ribbon) {
		const float baseY = h * ofLerp(0.18f, 0.54f, ribbon / 3.0f);
		const float amp = h * ofLerp(0.025f, 0.075f, ribbon / 3.0f);
		const float thickness = ofLerp(22.0f, 46.0f, ribbon / 3.0f);
		const float drift = t * ofLerp(0.08f, 0.18f, ribbon / 3.0f);
		ofMesh band;
		band.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

		for (int i = 0; i <= 56; ++i) {
			const float pct = i / 56.0f;
			const float x = ofLerp(-w * 0.14f, w * 1.14f, pct);
			const float wave = sin(pct * TWO_PI * (1.15f + ribbon * 0.35f) + drift + ribbon * 1.7f);
			const float y = baseY + wave * amp + sin(pct * TWO_PI * 3.0f - drift * 1.8f) * amp * 0.38f;
			const float edge = 1.0f - abs(pct - 0.5f) * 1.7f;
			const float alpha = ofClamp(edge, 0.0f, 1.0f) * ofLerp(18.0f, 34.0f, ribbon / 3.0f);
			const ofColor c = ribbon % 2 == 0 ? ofColor(55, 215, 255, alpha) : ofColor(255, 55, 210, alpha);
			band.addColor(c);
			band.addVertex(glm::vec3(x, y - thickness, 0));
			band.addColor(c);
			band.addVertex(glm::vec3(x, y + thickness, 0));
		}
		band.draw();
	}
	ofPopStyle();
}

void ofApp::drawHorizonGlow() {
	const float w = ofGetWidth();
	const float h = ofGetHeight();
	const float horizonY = h * 0.61f;
	const float t = ofGetElapsedTimef();

	ofPushStyle();
	ofFill();
	ofMesh glow;
	glow.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	for (int i = 0; i <= 48; ++i) {
		const float pct = i / 48.0f;
		const float x = pct * w;
		const float wave = sin(pct * TWO_PI * 2.0f + t * 0.45f) * 10.0f;
		const float alpha = 42.0f * (1.0f - abs(pct - 0.5f) * 0.9f);
		glow.addColor(ofColor(80, 220, 255, alpha));
		glow.addVertex(glm::vec3(x, horizonY - 18.0f + wave * 0.15f, 0));
		glow.addColor(ofColor(255, 45, 215, alpha * 0.55f));
		glow.addVertex(glm::vec3(x, horizonY + h * 0.18f + wave, 0));
	}
	glow.draw();

	for (int i = 0; i < 5; ++i) {
		const float y = horizonY + i * 9.0f;
		const float alpha = ofMap(i, 0, 4, 95.0f, 18.0f);
		ofSetColor(i % 2 == 0 ? ofColor(80, 230, 255, alpha) : ofColor(255, 65, 210, alpha));
		ofSetLineWidth(ofLerp(2.4f, 0.7f, i / 4.0f));
		ofDrawLine(w * 0.08f, y, w * 0.92f, y);
	}
	ofPopStyle();
}

void ofApp::drawSynthwaveGrid() {
	const float w = ofGetWidth();
	const float h = ofGetHeight();
	const float horizonY = h * 0.50f;
	const float centerX = w * 0.5f;
	const float t = ofGetElapsedTimef();
	const float bottomY = h + 100.0f;

	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(1.3f);

	for (int i = -16; i <= 16; ++i) {
		const float bottomX = centerX + i * (w * 0.064f);
		const float alpha = ofMap(abs(i), 0, 16, 170.0f, 34.0f, true);
		ofSetColor(55, 230, 255, alpha);
		ofSetLineWidth(ofMap(abs(i), 0, 16, 2.2f, 0.8f, true));
		ofDrawLine(centerX, horizonY, bottomX, bottomY);
	}

	for (int i = 0; i < 32; ++i) {
		const float phase = fmod(i / 32.0f + t * 0.62f, 1.0f);
		const float depth = phase * phase;
		const float y = horizonY + depth * (h - horizonY + 120.0f);
		const float halfW = ofLerp(82.0f, w * 0.96f, depth);
		const float alpha = ofMap(depth, 0.0f, 1.0f, 24.0f, 225.0f, true);
		ofSetColor(255, 50, 220, alpha);
		ofSetLineWidth(ofLerp(0.7f, 3.8f, depth));
		ofDrawLine(centerX - halfW, y, centerX + halfW, y);
	}

	ofFill();
	ofSetColor(255, 60, 215, 24);
	ofDrawRectangle(0, horizonY, w, h - horizonY);
	ofPopStyle();
}

void ofApp::drawParticleTunnel() {
	const float w = ofGetWidth();
	const float h = ofGetHeight();
	const float t = ofGetElapsedTimef();
	const ofVec2f center(w * 0.5f, h * 0.48f);
	const float maxRadius = max(w, h) * 0.76f;

	ofPushStyle();
	ofSetLineWidth(1.2f);
	for (const auto& particle : tunnelParticles) {
		const float phase = fmod(particle.offset + t * particle.speed, 1.0f);
		const float depth = phase * phase;
		const float prevDepth = max(0.0f, phase - 0.045f);
		const ofVec2f direction(cos(particle.angle), sin(particle.angle) * 0.72f);
		const ofVec2f pos = center + direction * (maxRadius * particle.radius * depth);
		const ofVec2f prev = center + direction * (maxRadius * particle.radius * prevDepth * prevDepth);
		const float alpha = ofMap(depth, 0.0f, 1.0f, 18.0f, 235.0f, true);
		const float size = particle.size * ofLerp(0.45f, 2.2f, depth);

		ofSetColor(particle.color, alpha * 0.66f);
		ofDrawLine(prev, pos);
		ofSetColor(particle.color, alpha);
		ofDrawCircle(pos, size);
	}
	ofPopStyle();
}

void ofApp::drawCoverImage(const ofImage& image) {
	ofDisableLighting();
	const float imageW = image.getWidth();
	const float imageH = image.getHeight();
	const float scale = max(ofGetWidth() / imageW, ofGetHeight() / imageH);
	const float drawW = imageW * scale;
	const float drawH = imageH * scale;
	ofSetColor(255);
	image.draw((ofGetWidth() - drawW) * 0.5f, (ofGetHeight() - drawH) * 0.5f, drawW, drawH);
}

ofRectangle ofApp::startButtonRect() const {
	const float w = 410.0f;
	const float h = 82.0f;
	return ofRectangle(ofGetWidth() * 0.5f - w * 0.5f, ofGetHeight() * 0.76f - h * 0.5f, w, h);
}

ofRectangle ofApp::tutorialButtonRect() const {
	if (tutorialButtonImage.isAllocated()) {
		const float scale = std::min(0.72f, (ofGetWidth() * 0.58f) / tutorialButtonImage.getWidth());
		const float w = tutorialButtonImage.getWidth() * scale;
		const float h = tutorialButtonImage.getHeight() * scale;
		const ofVec2f center(ofGetWidth() * 0.5f, ofGetHeight() - 78.0f);
		return ofRectangle(center.x - w * 0.5f, center.y - h * 0.5f, w, h);
	}

	const float w = 430.0f;
	const float h = 68.0f;
	return ofRectangle(ofGetWidth() * 0.5f - w * 0.5f, ofGetHeight() - 112.0f, w, h);
}

bool ofApp::isTutorialButtonHit(int x, int y) const {
	const ofRectangle button = tutorialButtonRect();
	if (!button.inside(x, y)) {
		return false;
	}

	if (!tutorialButtonImage.isAllocated()) {
		return true;
	}

	const ofPixels& pixels = tutorialButtonImage.getPixels();
	if (pixels.getNumChannels() < 4) {
		return true;
	}

	const int px = ofClamp(static_cast<int>((x - button.x) / button.width * pixels.getWidth()), 0, pixels.getWidth() - 1);
	const int py = ofClamp(static_cast<int>((y - button.y) / button.height * pixels.getHeight()), 0, pixels.getHeight() - 1);
	return pixels.getColor(px, py).a > 24;
}

void ofApp::startGame() {
	tutorialActive = false;
	gameStarted = true;
	resetGame();
	triggerSound(3, 1.0f, 0.5f);
}

void ofApp::drawStartMenu() {
	ofDisableLighting();
	ofDisableDepthTest();
	ofDisableBlendMode();
	ofEnableAlphaBlending();
	ofSetColor(255);
	ofFill();
	const bool showingVictoryScreen = gameWon && victoryScreenTimer > 0.0f;
	const ofImage* menuImage = (showingVictoryScreen && victoryImage.isAllocated()) ? &victoryImage : &introImage;
	if (menuImage->isAllocated()) {
		drawCoverImage(*menuImage);
	} else {
		ofBackgroundGradient(ofColor(18, 5, 42), ofColor(4, 0, 16));
	}

	ofPushStyle();
	if (showingVictoryScreen) {
		if (!victoryImage.isAllocated()) {
			ofSetColor(255, 230, 90, 235);
			ofDrawBitmapStringHighlight("VITORIA // BOSS DERROTADO", ofGetWidth() * 0.5f - 92.0f, ofGetHeight() * 0.5f, ofColor(0, 0, 0, 140), ofColor(255, 230, 90));
		}
		ofPopStyle();
		return;
	}

	if (menuImage->isAllocated()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofSetColor(255, 255, 255, 54);
		drawCoverImage(*menuImage);
	}

	const ofRectangle button = startButtonRect();
	const bool hover = button.inside(mousePosition);
	const float t = ofGetElapsedTimef();
	const float pulse = ofMap(sinf(t * 5.0f), -1.0f, 1.0f, 0.55f, 1.0f);

	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofSetColor(255, 40, 220, hover ? 70 : 34);
	ofDrawRectangle(button.x - 20, button.y - 12, button.width + 40, button.height + 24);
	ofSetColor(30, 230, 255, hover ? 64 : 32);
	ofDrawRectangle(button.x - 9, button.y - 20, button.width + 18, button.height + 40);

	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(6, 0, 24, 225);
	ofDrawRectangle(button);
	ofSetColor(80, 235, 255, hover ? 44 : 28);
	ofDrawRectangle(button.x + 12.0f, button.y + 12.0f, button.width - 24.0f, button.height - 24.0f);
	ofNoFill();
	ofSetLineWidth(3.5f);
	ofSetColor(55, 235, 255, hover ? 255 : 185);
	ofDrawRectangle(button);
	ofSetColor(255, 52, 220, 190 * pulse);
	ofDrawLine(button.x + 22, button.y - 8, button.x + button.width - 22, button.y - 8);
	ofDrawLine(button.x + 22, button.y + button.height + 8, button.x + button.width - 22, button.y + button.height + 8);

	ofSetLineWidth(5.0f);
	ofDrawLine(button.x - 28, button.y + 16, button.x + 24, button.y + 16);
	ofDrawLine(button.x + button.width - 24, button.y + button.height - 16, button.x + button.width + 28, button.y + button.height - 16);
	ofFill();

	const string label = "INICIAR // PLAY";
	if (startButtonFontLoaded) {
		const ofRectangle labelBounds = startButtonFont.getStringBoundingBox(label, 0.0f, 0.0f);
		const float labelX = button.getCenter().x - labelBounds.width * 0.5f - labelBounds.x;
		const float labelY = button.getCenter().y - labelBounds.height * 0.5f - labelBounds.y + 1.0f;
		ofSetColor(0, 0, 0, 190);
		startButtonFont.drawString(label, labelX + 3.0f, labelY + 4.0f);
		ofSetColor(255, 40, 220, 80 * pulse);
		ofDrawRectangle(labelX - 12.0f, button.getCenter().y + 20.0f, labelBounds.width + 24.0f, 4.0f);
		ofSetColor(255);
		startButtonFont.drawString(label, labelX, labelY);
	} else {
		const float labelScale = 2.25f;
		const float labelW = label.size() * 8.0f * labelScale;
		const float labelX = button.getCenter().x - labelW * 0.5f;
		const float labelY = button.getCenter().y + 10.0f;
		ofPushMatrix();
		ofTranslate(labelX, labelY);
		ofScale(labelScale, labelScale);
		ofSetColor(255);
		ofDrawBitmapString(label, 0.0f, 0.0f);
		ofPopMatrix();
	}

	const float infoW = 520.0f;
	const float infoH = 38.0f;
	const float infoX = ofGetWidth() * 0.5f - infoW * 0.5f;
	const float infoY = button.y + button.height + 28.0f;
	ofSetColor(4, 0, 22, 120);
	ofDrawRectangle(infoX, infoY, infoW, infoH);
	ofNoFill();
	ofSetLineWidth(1.8f);
	ofSetColor(55, 235, 255, 120);
	ofDrawRectangle(infoX, infoY, infoW, infoH);
	ofSetColor(255, 45, 220, 100 * pulse);
	ofDrawLine(infoX + 16.0f, infoY - 4.0f, infoX + infoW - 16.0f, infoY - 4.0f);
	ofFill();
	ofSetColor(80, 235, 255, 210);
	ofDrawBitmapString("CLICK / ENTER  //  B TESTA BOSS  //  SPACE ESPECIAL", infoX + 60.0f, infoY + 24.0f);
	ofPopStyle();
	ofDisableBlendMode();
	ofEnableAlphaBlending();
}

void ofApp::drawTutorialScreen() {
	ofDisableLighting();
	ofDisableDepthTest();
	ofDisableBlendMode();
	ofEnableAlphaBlending();
	ofSetColor(255);
	ofFill();

	if (tutorialImage.isAllocated()) {
		drawCoverImage(tutorialImage);

		const ofRectangle button = tutorialButtonRect();
		const bool hover = button.inside(mousePosition);
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofSetColor(55, 235, 255, hover ? 82 : 42);
		ofDrawRectangle(button.x - 10.0f, button.y - 8.0f, button.width + 20.0f, button.height + 16.0f);
		ofSetColor(255, 45, 220, hover ? 68 : 28);
		ofDrawRectangle(button.x - 18.0f, button.y - 5.0f, button.width + 36.0f, button.height + 10.0f);
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);

		if (tutorialButtonImage.isAllocated()) {
			ofSetColor(hover ? ofColor(255) : ofColor(238, 246, 255));
			tutorialButtonImage.draw(button);
		} else {
			ofSetColor(6, 0, 24, 225);
			ofDrawRectangle(button);
			ofNoFill();
			ofSetLineWidth(3.0f);
			ofSetColor(55, 235, 255, hover ? 255 : 190);
			ofDrawRectangle(button);
			ofFill();
			ofSetColor(255);
			ofDrawBitmapString("INICIAR", button.x + button.width * 0.5f - 28.0f, button.y + button.height * 0.5f + 5.0f);
		}

		ofSetColor(80, 235, 255, 185);
		ofDrawBitmapString("ENTER / SPACE tambem inicia  //  ESC volta ao menu", ofGetWidth() * 0.5f - 178.0f, button.y + button.height + 26.0f);
		ofPopStyle();
		ofDisableBlendMode();
		ofEnableAlphaBlending();
		return;
	}

	if (introImage.isAllocated()) {
		drawCoverImage(introImage);
	} else {
		ofBackgroundGradient(ofColor(18, 5, 42), ofColor(4, 0, 16));
	}

	const float w = ofGetWidth();
	const float h = ofGetHeight();
	const float t = ofGetElapsedTimef();
	const float pulse = ofMap(sinf(t * 5.2f), -1.0f, 1.0f, 0.55f, 1.0f);

	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(3, 0, 16, 196);
	ofDrawRectangle(0, 0, w, h);
	ofSetColor(255, 35, 220, 28 * pulse);
	ofDrawRectangle(0, 0, w, h);

	const float panelX = w * 0.08f;
	const float panelY = h * 0.09f;
	const float panelW = w * 0.84f;
	const float panelH = h * 0.72f;
	ofSetColor(4, 0, 22, 212);
	ofDrawRectangle(panelX, panelY, panelW, panelH);
	ofNoFill();
	ofSetLineWidth(3.5f);
	ofSetColor(55, 235, 255, 220);
	ofDrawRectangle(panelX, panelY, panelW, panelH);
	ofSetColor(255, 45, 220, 160 * pulse);
	ofDrawLine(panelX + 28.0f, panelY - 9.0f, panelX + panelW - 28.0f, panelY - 9.0f);
	ofDrawLine(panelX + 28.0f, panelY + panelH + 9.0f, panelX + panelW - 28.0f, panelY + panelH + 9.0f);
	ofFill();

	const string title = "GUIA RAPIDO // SOBREVIVA AO SETOR";
	if (warningSubtitleFontLoaded) {
		const ofRectangle titleBounds = warningSubtitleFont.getStringBoundingBox(title, 0.0f, 0.0f);
		const float titleScale = std::min(1.0f, (panelW - 120.0f) / titleBounds.width);
		const float titleX = w * 0.5f - titleBounds.width * titleScale * 0.5f - titleBounds.x * titleScale;
		ofPushMatrix();
		ofTranslate(titleX, panelY + 58.0f);
		ofScale(titleScale, titleScale);
		ofSetColor(0, 0, 0, 200);
		warningSubtitleFont.drawString(title, 3.0f, 4.0f);
		ofSetColor(255, 45, 220, 95 * pulse);
		warningSubtitleFont.drawString(title, -2.0f, 0.0f);
		ofSetColor(80, 235, 255, 95);
		warningSubtitleFont.drawString(title, 2.0f, -1.0f);
		ofSetColor(255);
		warningSubtitleFont.drawString(title, 0.0f, 0.0f);
		ofPopMatrix();
	} else {
		ofSetColor(255);
		ofDrawBitmapStringHighlight(title, panelX + 36.0f, panelY + 48.0f, ofColor(0, 0, 0, 120), ofColor(255));
	}

	auto drawCard = [&](float x, float y, float cw, float ch, const ofColor& color, const string& head, const vector<string>& lines) {
		ofSetColor(5, 2, 28, 190);
		ofDrawRectangle(x, y, cw, ch);
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofSetColor(color, 22);
		ofDrawRectangle(x - 6.0f, y - 6.0f, cw + 12.0f, ch + 12.0f);
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofSetColor(color, 44);
		ofDrawRectangle(x + cw - 8.0f, y, 8.0f, ch);
		ofNoFill();
		ofSetLineWidth(2.0f);
		ofSetColor(color, 205);
		ofDrawRectangle(x, y, cw, ch);
		ofSetLineWidth(4.0f);
		ofDrawLine(x - 2.0f, y + 24.0f, x + 28.0f, y - 2.0f);
		ofDrawLine(x + cw - 28.0f, y + ch + 2.0f, x + cw + 2.0f, y + ch - 24.0f);
		ofFill();

		if (tutorialHeaderFontLoaded) {
			ofSetColor(0, 0, 0, 180);
			tutorialHeaderFont.drawString(head, x + 21.0f, y + 31.0f);
			ofSetColor(color, 255);
			tutorialHeaderFont.drawString(head, x + 20.0f, y + 29.0f);
		} else {
			ofSetColor(color, 230);
			ofDrawBitmapString(head, x + 20.0f, y + 28.0f);
		}

		ofSetColor(color, 135);
		ofDrawRectangle(x + 20.0f, y + 44.0f, cw - 54.0f, 3.0f);
		for (size_t i = 0; i < lines.size(); ++i) {
			const float lineY = y + 72.0f + i * 28.0f;
			ofSetColor(color, 210);
			ofDrawRectangle(x + 22.0f, lineY - 11.0f, 9.0f, 9.0f);
			ofSetColor(255, 255, 255, 232);
			if (tutorialBodyFontLoaded) {
				tutorialBodyFont.drawString(lines[i], x + 42.0f, lineY);
			} else {
				ofDrawBitmapString(lines[i], x + 42.0f, lineY);
			}
		}
	};

	const float gap = 22.0f;
	const float cardW = (panelW - 72.0f - gap) * 0.5f;
	const float cardH = (panelH - 138.0f - gap) * 0.5f;
	const float cardX = panelX + 36.0f;
	const float cardY = panelY + 92.0f;
	drawCard(cardX, cardY, cardW, cardH, ofColor(80, 235, 255), "MIRA E TIROS", {"Segure a mira sobre alvos.", "A nave dispara sozinha.", "SPACE usa o especial carregado."});
	drawCard(cardX + cardW + gap, cardY, cardW, cardH, ofColor(255, 82, 45), "METEOROS", {"Destrua antes de chegar perto.", "Vermelhos sao mais fortes.", "Combo aumenta score e poder."});
	drawCard(cardX, cardY + cardH + gap, cardW, cardH, ofColor(255, 230, 70), "POWER UPS", {"Neon POWER UP = colete mirando.", "Escudo, boost, vida e tiro x2.", "Alguns carregam o especial."});
	drawCard(cardX + cardW + gap, cardY + cardH + gap, cardW, cardH, ofColor(255, 45, 220), "BOSS", {"Aparece apos o alerta CUIDADO.", "Destrua esferas para abrir dano.", "Encoste nas miras ou perde vida."});

	const ofRectangle button = tutorialButtonRect();
	const bool hover = button.inside(mousePosition);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofSetColor(255, 45, 220, hover ? 72 : 38);
	ofDrawRectangle(button.x - 14.0f, button.y - 9.0f, button.width + 28.0f, button.height + 18.0f);
	ofSetColor(55, 235, 255, hover ? 75 : 40);
	ofDrawRectangle(button.x - 7.0f, button.y - 15.0f, button.width + 14.0f, button.height + 30.0f);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(6, 0, 24, 230);
	ofDrawRectangle(button);
	ofNoFill();
	ofSetLineWidth(3.0f);
	ofSetColor(55, 235, 255, hover ? 255 : 190);
	ofDrawRectangle(button);
	ofFill();

	const string label = "ENTENDI // INICIAR";
	if (startButtonFontLoaded) {
		const ofRectangle labelBounds = startButtonFont.getStringBoundingBox(label, 0.0f, 0.0f);
		const float labelX = button.getCenter().x - labelBounds.width * 0.5f - labelBounds.x;
		const float labelY = button.getCenter().y - labelBounds.height * 0.5f - labelBounds.y + 1.0f;
		ofSetColor(0, 0, 0, 190);
		startButtonFont.drawString(label, labelX + 3.0f, labelY + 4.0f);
		ofSetColor(255);
		startButtonFont.drawString(label, labelX, labelY);
	} else {
		ofSetColor(255);
		ofDrawBitmapString(label, button.x + 134.0f, button.y + 42.0f);
	}

	ofSetColor(80, 235, 255, 190);
	ofDrawBitmapString("ENTER / SPACE tambem inicia  //  ESC volta ao menu", w * 0.5f - 178.0f, button.y + button.height + 28.0f);
	ofPopStyle();
}

void ofApp::drawPortalOverlay() {
	if (useProceduralBackground) {
		return;
	}

	const ofVec2f center(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);
	const float t = ofGetElapsedTimef();

	ofPushStyle();
	ofNoFill();
	for (int i = 0; i < 8; ++i) {
		const float phase = fmod(t * 0.32f + i / 8.0f, 1.0f);
		const float radiusX = ofLerp(70.0f, ofGetWidth() * 0.72f, phase);
		const float radiusY = radiusX * 0.48f;
		const float alpha = ofMap(phase, 0.0f, 1.0f, useProceduralBackground ? 95.0f : 150.0f, 0.0f, true);
		ofSetColor(150, 110, 255, alpha);
		ofSetLineWidth(ofLerp(3.5f, 0.8f, phase));
		ofDrawEllipse(center, radiusX, radiusY);
	}
	ofFill();

	ofMesh vignette;
	vignette.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
	vignette.addColor(ofColor(0, 0, 0, 0));
	vignette.addVertex(glm::vec3(center.x, center.y, 0));
	const float radius = max(ofGetWidth(), ofGetHeight()) * 0.78f;
	for (int i = 0; i <= 64; ++i) {
		const float a = ofMap(i, 0, 64, 0, TWO_PI);
		vignette.addColor(ofColor(0, 0, 20, useProceduralBackground ? 95 : 175));
		vignette.addVertex(glm::vec3(center.x + cos(a) * radius, center.y + sin(a) * radius, 0));
	}
	vignette.draw();
	ofPopStyle();
}

void ofApp::drawMeteor(const Meteor& meteor) {
	const float depth01 = ofMap(meteor.position.z, farZ, nearZ, 0.0f, 1.0f, true);
	const float pulse = isMouseOver(meteor) ? 1.0f + sin(ofGetElapsedTimef() * 18.0f) * 0.08f : 1.0f;
	const float explosionScale = meteor.exploding ? ofMap(meteor.explosion, 0.0f, 0.42f, 1.0f, 2.8f, true) : 1.0f;
	const float alpha = meteor.exploding ? ofMap(meteor.explosion, 0.0f, 0.42f, 255.0f, 0.0f, true) : 255.0f;

	ofPushMatrix();
	ofTranslate(meteor.position);
	ofRotateDeg(meteor.rotation, 0.4f, 1.0f, 0.15f);
	ofScale(pulse * explosionScale);
	ofSetColor(meteor.color, alpha);
	if (modelLoaded) {
		ofPushMatrix();
		if (meteorUsesRawModel) {
			const glm::vec3 meteorCenter = meteorModel.getSceneCenterModelSpace();
			ofScale((meteor.radius * 2.0f) / 3.65f);
			ofTranslate(-meteorCenter.x, -meteorCenter.y, -meteorCenter.z);
		} else {
			ofRotateDeg(90, 1, 0, 0);
			ofScale(meteor.radius * 1.25f);
		}
		meteorModel.drawFaces();
		if (meteorUsesRawModel) {
			ofSetColor(255, 190);
			meteorModel.drawWireframe();
		}
		if (meteor.heavy && !meteor.exploding) {
			const float redBlink = ofMap(sinf(ofGetElapsedTimef() * 9.0f + meteor.motionPhase), -1.0f, 1.0f, 0.15f, 1.0f);
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			ofSetColor(255, 24, 18, 95 * redBlink);
			meteorModel.drawFaces();
			ofSetColor(255, 0, 0, 210 * redBlink);
			ofSetLineWidth(2.0f);
			meteorModel.drawWireframe();
			ofPopStyle();
		}
		ofPopMatrix();
	} else {
		ofDrawSphere(0, 0, 0, meteor.radius);
	}

	if (!meteorUsesRawModel) {
		ofSetColor(255, 140 * (1.0f - depth01));
		ofNoFill();
		ofDrawSphere(0, 0, 0, meteor.radius * 1.08f);
		ofFill();
	}

	if (meteor.heavy && !meteor.exploding) {
		const float warningPulse = 0.75f + sin(ofGetElapsedTimef() * 5.5f + meteor.motionPhase) * 0.25f;
		ofPushStyle();
		ofNoFill();
		ofSetLineWidth(3.0f);
		for (int i = 0; i < 16; ++i) {
			const float a = meteor.motionPhase + ofGetElapsedTimef() * 0.85f + i * TWO_PI / 16.0f;
			const float r = meteor.radius * (1.18f + (i % 4 == 0 ? 0.08f : 0.0f));
			const ofVec3f center(cos(a) * r, sin(a) * r, 0);
			const ofVec3f tangent(-sin(a), cos(a), 0);
			const ofVec3f radial(cos(a), sin(a), 0);
			const float dash = meteor.radius * (i % 4 == 0 ? 0.34f : 0.20f);
			ofSetColor(i % 4 == 0 ? ofColor(255, 48, 235, 220 * warningPulse) : ofColor(255, 74, 28, 165 * warningPulse));
			ofDrawLine(center - tangent * dash, center + tangent * dash);
			ofSetColor(255, 175, 38, 180 * warningPulse);
			ofDrawLine(center + radial * meteor.radius * 0.04f, center + radial * meteor.radius * 0.24f);
		}

		ofFill();
		for (int i = 0; i < 4; ++i) {
			const float a = meteor.motionPhase * 0.5f + i * HALF_PI;
			ofPushMatrix();
			ofTranslate(cos(a) * meteor.radius * 1.42f, sin(a) * meteor.radius * 1.42f, 0);
			ofRotateDeg(ofRadToDeg(a) + 45.0f, 0, 0, 1);
			ofSetColor(60, 230, 255, 95 * warningPulse);
			ofDrawBox(0, 0, 0, meteor.radius * 0.36f, meteor.radius * 0.08f, meteor.radius * 0.05f);
			ofSetColor(255, 45, 220, 150 * warningPulse);
			ofDrawBox(meteor.radius * 0.18f, 0, 0, meteor.radius * 0.08f, meteor.radius * 0.16f, meteor.radius * 0.08f);
			ofPopMatrix();
		}

		const float pipSize = meteor.radius * 0.16f;
		const float startX = -(meteor.maxHitPoints - 1) * pipSize * 0.68f;
		for (int i = 0; i < meteor.maxHitPoints; ++i) {
			const bool active = i < meteor.hitPoints;
			ofSetColor(active ? ofColor(255, 80, 35, 220) : ofColor(80, 35, 35, 120));
			ofDrawBox(startX + i * pipSize * 1.35f, -meteor.radius * 1.28f, 0, pipSize, pipSize * 0.45f, pipSize * 0.45f);
		}
		ofPopStyle();
	}

	if (meteor.dwell > 0.0f && !meteor.exploding) {
		const float damageProgress = ofClamp(meteor.dwell / dwellToExplode, 0.0f, 1.0f);
		ofPushStyle();
		ofNoFill();
		ofSetLineWidth(ofLerp(2.0f, 4.2f, damageProgress));
		for (int i = 0; i < 18; ++i) {
			const float segmentStart = i / 18.0f;
			const bool active = segmentStart <= damageProgress;
			const float a = meteor.motionPhase + i * TWO_PI / 18.0f + ofGetElapsedTimef() * 1.3f;
			const float r = meteor.radius * ofLerp(0.98f, 1.28f, damageProgress);
			const ofVec3f center(cos(a) * r, sin(a) * r, 0);
			const ofVec3f tangent(-sin(a), cos(a), 0);
			const ofVec3f radial(cos(a), sin(a), 0);
			const float dash = meteor.radius * (active ? 0.24f : 0.12f);
			ofSetColor(active ? ofColor(255, 225, 70, 245) : ofColor(70, 210, 255, 72));
			ofDrawLine(center - tangent * dash, center + tangent * dash);

			if (active && i % 3 == 0) {
				ofSetColor(255, 85, 25, 175);
				ofDrawLine(center - radial * meteor.radius * 0.16f, center + radial * meteor.radius * 0.12f);
			}
		}

		ofFill();
		for (int i = 0; i < 5; ++i) {
			const float a = meteor.motionPhase + damageProgress * TWO_PI + i * 0.34f;
			const float r = meteor.radius * ofLerp(0.65f, 1.05f, damageProgress);
			ofSetColor(255, 120, 32, 180 * damageProgress);
			ofDrawBox(cos(a) * r, sin(a) * r, 0, meteor.radius * 0.07f, meteor.radius * 0.07f, meteor.radius * 0.07f);
		}
		ofPopStyle();
	}
	ofPopMatrix();
}

void ofApp::drawBoss() {
	if (!bossActive) {
		return;
	}

	const ofVec3f bossPosition = bossWorldPosition();
	const float introProgress = bossIntroTimer > 0.0f ? 1.0f - ofClamp(bossIntroTimer / 2.2f, 0.0f, 1.0f) : 1.0f;
	const float introEase = introProgress * introProgress * (3.0f - 2.0f * introProgress);
	const float pulse = bossVulnerable ? ofMap(sinf(ofGetElapsedTimef() * 6.5f), -1.0f, 1.0f, 0.98f, 1.03f) : 1.0f;
	const float fightYaw = sinf(ofGetElapsedTimef() * 0.95f) * (bossVulnerable ? 6.0f : 15.0f);
	const ofColor coreColor = bossVulnerable ? ofColor(75, 245, 255) : ofColor(255, 45, 95);
	const ofColor wireColor = bossVulnerable ? ofColor(255, 230, 90) : ofColor(255, 55, 230);

	ofPushMatrix();
	ofTranslate(bossPosition.x, bossPosition.y, bossPosition.z + ofLerp(-780.0f, 0.0f, introEase));
	ofScale(pulse * ofLerp(0.32f, 1.0f, introEase));
	if (bossModelLoaded) {
		ofPushMatrix();
		ofRotateDeg(bossModelRotationX, 1, 0, 0);
		ofRotateDeg(bossModelRotationY + fightYaw, 0, 1, 0);
		ofRotateDeg(bossModelRotationZ, 0, 0, 1);
		ofScale(bossModelScale);
		const glm::vec3 bossCenter = bossModel.getSceneCenterModelSpace();
		ofTranslate(-bossCenter.x, -bossCenter.y, -bossCenter.z);
		ofSetColor(255, 245);
		if (bossProceduralTextureReady) {
			bossProceduralTexture.getTexture().bind();
		}
		bossModel.drawFaces();
		if (bossProceduralTextureReady) {
			bossProceduralTexture.getTexture().unbind();
		}

		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		bossModel.disableMaterials();
		bossModel.disableTextures();
		ofSetColor(coreColor, bossVulnerable ? 96 : 48);
		bossModel.drawFaces();
		ofSetColor(wireColor, bossVulnerable ? 235 : 170);
		ofSetLineWidth(2.0f);
		bossModel.drawWireframe();
		ofPopStyle();
		ofPopMatrix();
	} else {
		ofSetColor(coreColor, bossVulnerable ? 220 : 205);
		ofDrawSphere(0, 0, 0, bossRadius);
	}

	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(3.0f);
	ofSetColor(wireColor, 190);
	if (bossModelLoaded) {
		ofDrawBox(0, 0, 0, bossRadius * 2.15f, bossRadius * 1.45f, bossRadius * 1.45f);
	} else {
		ofDrawSphere(0, 0, 0, bossRadius * 1.06f);
	}
	ofSetLineWidth(2.0f);
	ofSetColor(80, 235, 255, bossVulnerable ? 210 : 110);
	for (int i = 0; i < 12; ++i) {
		const float a = ofGetElapsedTimef() * 0.65f + i * TWO_PI / 12.0f;
		const ofVec3f p(cos(a) * bossRadius * 1.22f, sin(a) * bossRadius * 0.72f, sin(a * 1.7f) * 38.0f);
		ofDrawBox(p, bossRadius * 0.13f, bossRadius * 0.035f, bossRadius * 0.035f);
	}
	ofPopStyle();

	if (bossIntroTimer > 0.0f) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofNoFill();
		for (int i = 0; i < 7; ++i) {
			const float ringProgress = fmod(introProgress + i * 0.14f, 1.0f);
			const float ringRadius = ofLerp(bossRadius * 0.38f, bossRadius * 2.25f, ringProgress);
			const float alpha = ofMap(ringProgress, 0.0f, 1.0f, 235.0f, 0.0f, true);
			ofSetLineWidth(ofLerp(6.0f, 1.5f, ringProgress));
			ofSetColor(i % 2 == 0 ? ofColor(255, 45, 95, alpha) : ofColor(80, 235, 255, alpha));
			ofDrawCircle(0, 0, ringRadius);
		}
		ofFill();
		for (int i = 0; i < 18; ++i) {
			const float a = ofGetElapsedTimef() * 4.0f + i * TWO_PI / 18.0f;
			const float r = bossRadius * ofLerp(1.6f, 2.5f, introProgress);
			ofSetColor(255, 230, 90, 150 * (1.0f - introProgress));
			ofDrawBox(cos(a) * r, sin(a) * r * 0.62f, sin(a * 1.7f) * 80.0f, 18, 18, 18);
		}
		ofPopStyle();
	}

	if (bossVulnerable && bossDwell > 0.0f) {
		const float progress = ofClamp(bossDwell / 0.72f, 0.0f, 1.0f);
		ofPushStyle();
		ofNoFill();
		ofSetLineWidth(5.0f);
		ofSetColor(255, 230, 70, 230);
		ofDrawCircle(0, 0, bossRadius * ofLerp(1.16f, 1.42f, progress));
		ofFill();
		ofSetColor(255, 60, 220, 190);
		for (int i = 0; i < 8; ++i) {
			const float a = i * TWO_PI / 8.0f + progress * TWO_PI;
			ofDrawBox(cos(a) * bossRadius * 1.32f, sin(a) * bossRadius * 1.32f, 0, 18, 18, 18);
		}
		ofPopStyle();
	}

	ofPopMatrix();
}

void ofApp::drawBossOrbs() {
	if (!bossActive) {
		return;
	}

	for (const auto& orb : bossOrbs) {
		const float explosionScale = orb.exploding ? ofMap(orb.explosion, 0.0f, 0.35f, 1.0f, 2.9f, true) : 1.0f;
		const float alpha = orb.exploding ? ofMap(orb.explosion, 0.0f, 0.35f, 255.0f, 0.0f, true) : 255.0f;
		ofPushMatrix();
		ofTranslate(orb.position);
		ofRotateDeg(ofGetElapsedTimef() * 90.0f + orb.phase, 0.2f, 1.0f, 0.4f);
		ofScale(explosionScale);
		ofSetColor(orb.color, alpha);
		ofDrawSphere(0, 0, 0, orb.radius);
		ofPushStyle();
		ofNoFill();
		ofSetLineWidth(2.0f);
		ofSetColor(255, 230, 90, alpha * 0.72f);
		ofDrawSphere(0, 0, 0, orb.radius * 1.25f);
		if (orb.dwell > 0.0f && !orb.exploding) {
			ofSetColor(80, 235, 255, 220);
			ofDrawBox(0, -orb.radius * 1.55f, 0, orb.radius * 1.3f * ofClamp(orb.dwell / 0.34f, 0.0f, 1.0f), 5.0f, 5.0f);
		}
		ofPopStyle();
		ofPopMatrix();
	}
}

void ofApp::drawBossTargets() {
	if (!bossActive) {
		return;
	}

	ofPushStyle();
	ofDisableDepthTest();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	for (const auto& target : bossTargets) {
		if (target.resolved) {
			continue;
		}

		const float progress = ofClamp(target.age / target.duration, 0.0f, 1.0f);
		const float danger = 1.0f - progress;
		const float blink = ofMap(sinf(ofGetElapsedTimef() * 12.0f + target.phase), -1.0f, 1.0f, 0.45f, 1.0f);
		const float outerRadius = target.radius * ofLerp(2.15f, 0.92f, progress);

		ofNoFill();
		ofSetLineWidth(4.0f);
		ofSetColor(255, 40, 40, 180 * blink);
		ofDrawCircle(target.position, outerRadius);
		ofSetColor(255, 225, 40, 230);
		ofDrawCircle(target.position, target.radius);

		ofSetLineWidth(2.5f);
		ofDrawLine(target.position.x - target.radius * 1.35f, target.position.y, target.position.x - target.radius * 0.45f, target.position.y);
		ofDrawLine(target.position.x + target.radius * 0.45f, target.position.y, target.position.x + target.radius * 1.35f, target.position.y);
		ofDrawLine(target.position.x, target.position.y - target.radius * 1.35f, target.position.x, target.position.y - target.radius * 0.45f);
		ofDrawLine(target.position.x, target.position.y + target.radius * 0.45f, target.position.x, target.position.y + target.radius * 1.35f);

		ofFill();
		ofSetColor(255, 45, 220, 95 * blink);
		ofDrawRectangle(target.position.x - target.radius, target.position.y - target.radius - 12.0f, target.radius * 2.0f * danger, 6.0f);
		ofSetColor(255, 240, 90, 220);
		ofDrawBitmapString("TOQUE A MIRA", target.position.x - 45.0f, target.position.y + target.radius + 24.0f);
	}
	ofPopStyle();
}

void ofApp::drawPowerUps() {
	ofPushStyle();
	ofDisableLighting();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	for (const auto& powerUp : powerUps) {
		if (powerUp.collected) {
			continue;
		}

		const float depth01 = ofMap(powerUp.position.z, farZ, nearZ, 0.0f, 1.0f, true);
		const float pulse = ofMap(sinf(ofGetElapsedTimef() * 6.0f + powerUp.phase), -1.0f, 1.0f, 0.82f, 1.16f);
		const float size = powerUp.radius * ofLerp(0.68f, 1.18f, depth01) * pulse;
		const float glitch = ofMap(sinf(ofGetElapsedTimef() * 11.0f + powerUp.phase), -1.0f, 1.0f, 0.25f, 1.0f);
		const ofColor neon = powerUp.color.getLerped(ofColor(80, 235, 255), 0.28f);
		const ofColor magenta = powerUp.color.getLerped(ofColor(255, 45, 220), 0.42f);

		ofPushMatrix();
		ofTranslate(powerUp.position);
		ofRotateDeg(ofGetElapsedTimef() * 75.0f + powerUp.phase * RAD_TO_DEG, 0.25f, 1.0f, 0.15f);
		ofSetColor(neon, 96);
		ofDrawSphere(0, 0, 0, size * 2.45f);
		ofSetColor(magenta, 82 * glitch);
		ofDrawSphere(0, 0, 0, size * 3.1f);
		ofSetColor(powerUp.color.getLerped(ofColor(255), 0.18f), 255);
		ofDrawSphere(0, 0, 0, size);
		ofSetColor(255, 255, 255, 90);
		ofDrawSphere(0, 0, 0, size * 0.62f);
		ofNoFill();
		ofSetLineWidth(3.2f);
		ofSetColor(powerUp.color.getLerped(ofColor(255), 0.55f), 255);
		ofDrawSphere(0, 0, 0, size * 1.36f);
		ofSetLineWidth(5.0f);
		ofSetColor(neon, 245);
		ofDrawCircle(0, 0, size * 1.75f);
		ofSetColor(magenta, 225 * glitch);
		ofDrawCircle(0, 0, size * (2.12f + glitch * 0.22f));
		ofSetLineWidth(3.2f);
		for (int i = 0; i < 12; ++i) {
			const float a = powerUp.phase + ofGetElapsedTimef() * 1.8f + i * TWO_PI / 12.0f;
			const ofVec3f center(cos(a) * size * 1.95f, sin(a) * size * 1.95f, sin(a * 1.4f) * size * 0.24f);
			const ofVec3f tangent(-sin(a), cos(a), 0.0f);
			const float dash = size * (i % 3 == 0 ? 0.34f : 0.18f);
			ofSetColor(i % 2 == 0 ? ofColor(80, 235, 255, 255 * glitch) : ofColor(255, 45, 220, 230 * glitch));
			ofDrawLine(center - tangent * dash, center + tangent * dash);
		}
		ofSetColor(255, 230, 90, 235);
		ofDrawBox(0, -size * 1.55f, 0, size * 1.55f * ofClamp(powerUp.dwell / 0.26f, 0.0f, 1.0f), 7.0f, 7.0f);

		ofFill();
		for (int i = 0; i < 6; ++i) {
			const float a = powerUp.phase * 0.6f + ofGetElapsedTimef() * 2.4f + i * TWO_PI / 6.0f;
			ofPushMatrix();
			ofTranslate(cos(a) * size * 2.18f, sin(a) * size * 2.18f, sin(a * 1.6f) * size * 0.38f);
			ofRotateDeg(ofRadToDeg(a) + 45.0f, 0, 0, 1);
			ofSetColor(i % 2 == 0 ? ofColor(255, 45, 220, 220 * glitch) : ofColor(80, 235, 255, 220 * glitch));
			ofDrawBox(0, 0, 0, size * 0.46f, size * 0.12f, size * 0.12f);
			ofPopMatrix();
		}
		ofSetColor(255, 255, 255, 255);
		if (powerUp.type == 0) {
			ofDrawBox(0, 0, 0, size * 0.22f, size * 1.15f, size * 0.22f);
			ofDrawBox(0, 0, 0, size * 1.15f, size * 0.22f, size * 0.22f);
		} else if (powerUp.type == 1) {
			ofNoFill();
			ofSetLineWidth(3.0f);
			ofDrawCircle(0, 0, size * 0.72f);
			ofFill();
		} else if (powerUp.type == 2) {
			ofDrawBox(0, 0, 0, size * 0.28f, size * 1.05f, size * 0.22f);
			ofDrawBox(0, 0, 0, size * 1.05f, size * 0.28f, size * 0.22f);
		} else if (powerUp.type == 3) {
			ofDrawBox(-size * 0.24f, 0, 0, size * 0.22f, size * 1.08f, size * 0.22f);
			ofDrawBox(size * 0.24f, 0, 0, size * 0.22f, size * 1.08f, size * 0.22f);
			ofDrawBox(0, -size * 0.48f, 0, size * 0.74f, size * 0.18f, size * 0.18f);
		} else {
			ofDrawTriangle(0, -size * 0.78f, size * 0.62f, size * 0.36f, -size * 0.62f, size * 0.36f);
		}
		ofPopMatrix();
	}
	ofEnableLighting();
	ofPopStyle();
}

void ofApp::drawPowerUpIndicators() {
	ofPushStyle();
	ofDisableDepthTest();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	for (const auto& powerUp : powerUps) {
		if (powerUp.collected) {
			continue;
		}

		const glm::vec3 screen3 = camera.worldToScreen(glm::vec3(powerUp.position.x, powerUp.position.y, powerUp.position.z));
		const ofVec2f screen(screen3.x, screen3.y);
		if (screen.x < -120.0f || screen.x > ofGetWidth() + 120.0f || screen.y < -120.0f || screen.y > ofGetHeight() + 120.0f) {
			continue;
		}

		const float depth01 = ofMap(powerUp.position.z, farZ, nearZ, 0.0f, 1.0f, true);
		const float blink = ofMap(sinf(ofGetElapsedTimef() * 8.5f + powerUp.phase), -1.0f, 1.0f, 0.55f, 1.0f);
		const float radius = ofLerp(34.0f, 78.0f, depth01) * (1.0f + blink * 0.12f);
		const ofColor neon = powerUp.color.getLerped(ofColor(80, 235, 255), 0.35f);
		const ofColor magenta = powerUp.color.getLerped(ofColor(255, 45, 220), 0.45f);

		ofFill();
		ofSetColor(neon, 70 * blink);
		ofDrawCircle(screen, radius * 1.55f);
		ofSetColor(magenta, 52 * blink);
		ofDrawCircle(screen, radius * 2.05f);

		ofNoFill();
		ofSetLineWidth(5.0f);
		ofSetColor(neon, 245 * blink);
		ofDrawCircle(screen, radius);
		ofSetLineWidth(2.5f);
		ofSetColor(magenta, 225 * blink);
		ofDrawCircle(screen, radius * 1.28f);

		for (int i = 0; i < 16; ++i) {
			const float a = powerUp.phase + ofGetElapsedTimef() * 2.5f + i * TWO_PI / 16.0f;
			const ofVec2f radial(cos(a), sin(a));
			const ofVec2f tangent(-sin(a), cos(a));
			const ofVec2f center = screen + radial * radius * (1.42f + (i % 4 == 0 ? 0.16f : 0.0f));
			const float dash = radius * (i % 4 == 0 ? 0.22f : 0.12f);
			ofSetColor(i % 2 == 0 ? ofColor(80, 235, 255, 230 * blink) : ofColor(255, 45, 220, 210 * blink));
			ofDrawLine(center - tangent * dash, center + tangent * dash);
		}

		ofFill();
		ofSetColor(255, 235, 100, 240 * blink);
		ofDrawBitmapStringHighlight("POWER UP", screen.x - 34.0f, screen.y - radius - 22.0f, ofColor(0, 0, 0, 150), ofColor(255, 235, 100, 240 * blink));
	}
	ofPopStyle();
}

void ofApp::drawParticles() {
	ofPushStyle();
	for (const auto& particle : particles) {
		const float t = ofClamp(particle.age / particle.lifetime, 0.0f, 1.0f);
		const float alpha = ofMap(t, 0.0f, 1.0f, 255.0f, 0.0f, true);
		ofSetColor(particle.color, alpha);
		ofDrawSphere(particle.position, particle.size * (1.0f + t * 1.8f));
	}
	ofPopStyle();
}

void ofApp::drawShip() {
	const float nx = ofMap(aimPosition.x, 0, ofGetWidth(), -1.0f, 1.0f, true);
	const float ny = ofMap(aimPosition.y, 0, ofGetHeight(), -1.0f, 1.0f, true);

	ofPushMatrix();
	ofTranslate(0, -210, 405);
	ofRotateDeg(nx * 18.0f, 0, 1, 0);
	ofRotateDeg(-nx * 28.0f, 0, 0, 1);
	ofRotateDeg(ny * 8.0f, 1, 0, 0);
	if (shipModelLoaded) {
		shipModel.disableMaterials();
		shipModel.disableTextures();
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofPushMatrix();
		ofTranslate(0, -68, -46);
		ofRotateDeg(-90, 1, 0, 0);
		ofRotateDeg(180, 0, 0, 1);
		ofScale(7.25f, -3.25f, 7.25f);
		const glm::vec3 reflectionCenter = shipModel.getSceneCenterModelSpace();
		ofTranslate(-reflectionCenter.x, -reflectionCenter.y, -reflectionCenter.z);
		ofSetColor(70, 185, 255, 78);
		shipModel.drawFaces();
		ofSetColor(255, 75, 230, 92);
		shipModel.drawWireframe();
		ofPopMatrix();
		ofPopStyle();
		shipModel.enableMaterials();
		shipModel.enableTextures();

		ofPushMatrix();
		ofRotateDeg(-90, 1, 0, 0);
		ofRotateDeg(180, 0, 0, 1);
		ofScale(6.7f);
		const glm::vec3 shipCenter = shipModel.getSceneCenterModelSpace();
		ofTranslate(-shipCenter.x, -shipCenter.y, -shipCenter.z);
		ofSetColor(255);
		shipModel.drawFaces();
		ofSetColor(185, 230, 255, 62);
		shipModel.drawWireframe();
		ofPopMatrix();

		ofSetColor(255, 145, 55, 210);
		ofDrawSphere(0, 36, 8, 7.5f + sin(ofGetElapsedTimef() * 11.0f) * 2.0f);
	} else {
		ofRotateDeg(180, 0, 1, 0);
		ofSetColor(80, 180, 255);
		ofDrawCone(0, 0, 0, 55, 150);
		ofSetColor(255, 120, 40);
		ofDrawCone(0, -75, 0, 24, 80);
		ofSetColor(255, 220, 120, 180);
		ofDrawSphere(0, -116, 0, 22 + sin(ofGetElapsedTimef() * 10.0f) * 5.0f);
	}

	if (shieldTimer > 0.0f) {
		const float shieldPulse = ofMap(sinf(ofGetElapsedTimef() * 7.0f), -1.0f, 1.0f, 0.72f, 1.0f);
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofNoFill();
		ofSetLineWidth(3.0f);
		ofSetColor(75, 255, 155, 130 * shieldPulse);
		ofDrawSphere(0, -12, 0, 118.0f);
		ofSetColor(80, 235, 255, 95 * shieldPulse);
		ofDrawCircle(0, -12, 134.0f + sinf(ofGetElapsedTimef() * 5.0f) * 8.0f);
		ofPopStyle();
	}

	if (heavyShotTimer > 0.0f) {
		const float shotPulse = ofMap(sinf(ofGetElapsedTimef() * 12.0f), -1.0f, 1.0f, 0.55f, 1.0f);
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofSetColor(255, 155, 45, 150 * shotPulse);
		ofDrawSphere(-26, 28, 18, 10.0f);
		ofDrawSphere(26, 28, 18, 10.0f);
		ofSetColor(255, 230, 90, 90 * shotPulse);
		ofDrawCone(-26, 40, 18, 8.0f, 46.0f);
		ofDrawCone(26, 40, 18, 8.0f, 46.0f);
		ofPopStyle();
	}
	ofPopMatrix();
}

void ofApp::drawMissiles() {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	for (const auto& missile : missiles) {
		const float t = ofClamp(missile.age / missile.duration, 0.0f, 1.0f);
		const float eased = 1.0f - powf(1.0f - t, 3.0f);
		const ofVec2f pos = missile.start.getInterpolated(missile.end, eased);
		const ofVec2f trail = missile.start.getInterpolated(missile.end, ofClamp(eased - 0.24f, 0.0f, 1.0f));
		ofVec2f dir = missile.end - missile.start;
		if (dir.lengthSquared() <= 0.001f) {
			continue;
		}
		dir.normalize();
		const ofVec2f side(-dir.y, dir.x);
		const float fade = 1.0f - t;
		const ofColor beamColor = missile.color.getLerped(ofColor(70, 235, 255), 0.34f);
		const ofColor hotColor = missile.color.getLerped(ofColor(255, 245, 210), 0.68f);

		ofSetLineWidth(16.0f);
		ofSetColor(beamColor, 42 * fade);
		ofDrawLine(trail, pos);
		ofSetLineWidth(8.0f);
		ofSetColor(beamColor, 112 * fade);
		ofDrawLine(trail + side * 2.0f, pos + side * 2.0f);
		ofSetLineWidth(3.0f);
		ofSetColor(hotColor, 245 * fade);
		ofDrawLine(trail, pos);

		ofSetLineWidth(2.0f);
		for (int i = 0; i < 7; ++i) {
			const float p0 = ofClamp(eased - 0.27f + i * 0.045f, 0.0f, eased);
			const float p1 = ofClamp(p0 + 0.018f, 0.0f, eased);
			const ofVec2f a = missile.start.getInterpolated(missile.end, p0);
			const ofVec2f b = missile.start.getInterpolated(missile.end, p1);
			const float rail = (i % 2 == 0) ? 9.0f : -9.0f;
			ofSetColor(i % 2 == 0 ? ofColor(255, 45, 220, 145 * fade) : ofColor(55, 235, 255, 145 * fade));
			ofDrawLine(a + side * rail, b + side * rail);
		}

		ofFill();
		ofSetColor(255, 245, 210, 245 * fade);
		ofDrawTriangle(pos + dir * 24.0f, pos - dir * 14.0f + side * 10.0f, pos - dir * 14.0f - side * 10.0f);
		ofSetColor(beamColor, 160 * fade);
		ofDrawTriangle(pos + dir * 32.0f, pos - dir * 4.0f + side * 18.0f, pos - dir * 4.0f - side * 18.0f);

		ofNoFill();
		ofSetLineWidth(2.0f);
		ofSetColor(255, 45, 220, 175 * fade);
		ofDrawRectangle(pos.x - 11.0f, pos.y - 11.0f, 22.0f, 22.0f);
		ofSetColor(55, 235, 255, 150 * fade);
		ofDrawLine(pos - side * 24.0f, pos - side * 11.0f);
		ofDrawLine(pos + side * 11.0f, pos + side * 24.0f);
		ofFill();
	}
	ofPopStyle();
}

void ofApp::drawCrosshair() {
	const float size = 64.0f;
	const float progress = currentDwellProgress();

	if (hasAssistedTarget) {
		ofSetColor(255, 210, 90, 88);
		ofNoFill();
		ofSetLineWidth(2.0f);
		ofDrawCircle(assistedTarget, 36 + sin(ofGetElapsedTimef() * 8.0f) * 4.0f);
		ofFill();
		ofSetLineWidth(1.0f);
	}

	if (crosshairImage.isAllocated()) {
		ofSetColor(255);
		crosshairImage.draw(aimPosition.x - size * 0.5f, aimPosition.y - size * 0.5f, size, size);
	} else {
		ofSetColor(255, 80, 70);
		ofNoFill();
		ofDrawCircle(aimPosition, 20);
		ofDrawLine(aimPosition.x - 34, aimPosition.y, aimPosition.x - 10, aimPosition.y);
		ofDrawLine(aimPosition.x + 10, aimPosition.y, aimPosition.x + 34, aimPosition.y);
		ofDrawLine(aimPosition.x, aimPosition.y - 34, aimPosition.x, aimPosition.y - 10);
		ofDrawLine(aimPosition.x, aimPosition.y + 10, aimPosition.x, aimPosition.y + 34);
		ofFill();
	}

	if (progress > 0.0f) {
		ofPushStyle();
		ofNoFill();
		ofSetLineWidth(5.0f);
		ofSetColor(255, 230, 80, 230);
		ofPolyline arc;
		const float radius = 42.0f;
		const int steps = 48;
		for (int i = 0; i <= steps * progress; ++i) {
			const float a = -HALF_PI + TWO_PI * (i / (float)steps);
			arc.addVertex(aimPosition.x + cos(a) * radius, aimPosition.y + sin(a) * radius);
		}
		arc.draw();
		ofPopStyle();
	}

	ofSetColor(255, 255, 255, 70);
	ofDrawCircle(mousePosition, 4);

	if (powerPulse > 0.0f) {
		ofPushStyle();
		ofNoFill();
		ofSetLineWidth(5.0f);
		ofSetColor(100, 210, 255, 210 * powerPulse);
		ofDrawCircle(aimPosition, ofMap(powerPulse, 1.0f, 0.0f, 55.0f, 330.0f));
		ofPopStyle();
	}
}

void ofApp::drawHud() {
	bool hasHeavyMeteor = false;
	float closestHeavyDepth = 0.0f;
	for (const auto& meteor : meteors) {
		if (meteor.heavy && !meteor.exploding) {
			hasHeavyMeteor = true;
			closestHeavyDepth = std::max(closestHeavyDepth, ofMap(meteor.position.z, farZ, nearZ, 0.0f, 1.0f, true));
		}
	}

	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);

	const float panelX = 18.0f;
	const float panelY = 18.0f;
	const float panelW = 420.0f;
	const float panelH = 172.0f;
	const float t = ofGetElapsedTimef();
	const float pulse = ofMap(sinf(t * 4.5f), -1.0f, 1.0f, 0.55f, 1.0f);

	ofSetColor(4, 0, 22, 150);
	ofDrawRectangle(panelX, panelY, panelW, panelH);
	ofSetColor(55, 235, 255, 38);
	ofDrawRectangle(panelX, panelY, 7.0f, panelH);
	ofNoFill();
	ofSetLineWidth(2.4f);
	ofSetColor(55, 235, 255, 190);
	ofDrawRectangle(panelX, panelY, panelW, panelH);
	ofSetColor(255, 45, 220, 120 * pulse);
	ofDrawLine(panelX + 18, panelY - 6, panelX + panelW - 18, panelY - 6);
	ofDrawLine(panelX + 18, panelY + panelH + 6, panelX + panelW - 18, panelY + panelH + 6);
	ofSetLineWidth(4.0f);
	ofDrawLine(panelX - 2.0f, panelY + 28.0f, panelX + 34.0f, panelY - 2.0f);
	ofDrawLine(panelX + panelW - 34.0f, panelY + panelH + 2.0f, panelX + panelW + 2.0f, panelY + panelH - 28.0f);
	ofFill();

	ofSetColor(255);
	ofDrawBitmapString("EXPERIENCIA // DEFESA ORBITAL", panelX + 18, panelY + 25);
	ofSetColor(80, 235, 255);
	ofDrawBitmapString("SCORE", panelX + 18, panelY + 52);
	ofSetColor(255);
	ofDrawBitmapString(ofToString(score), panelX + 92, panelY + 52);

	ofSetColor(255, 70, 220);
	ofDrawBitmapString("TEMPO", panelX + 218, panelY + 52);
	ofSetColor(255);
	ofDrawBitmapString(ofToString(gameTimer, 1) + "s", panelX + 292, panelY + 52);

	ofSetColor(80, 235, 255);
	ofDrawBitmapString("ONDA " + ofToString(wave), panelX + 18, panelY + 78);
	ofSetColor(255);
	ofDrawBitmapString("METEOROS " + ofToString(destroyedInWave) + "/" + ofToString(meteorsPerWave), panelX + 92, panelY + 78);
	ofSetColor(255, 210, 80);
	ofDrawBitmapString("COMBO x" + ofToString(combo), panelX + 286, panelY + 78);

	const float lifeX = panelX + 18;
	const float lifeY = panelY + 102;
	const float lifeW = 24.0f;
	const float lifeH = 13.0f;
	ofSetColor(255);
	ofDrawBitmapString("VIDAS", lifeX, lifeY - 8);
	for (int i = 0; i < maxLives; ++i) {
		const bool active = i < lives;
		const float x = lifeX + i * (lifeW + 6.0f);
		ofSetColor(active ? ofColor(55, 235, 255, 210) : ofColor(55, 25, 55, 145));
		ofDrawRectangle(x, lifeY, lifeW, lifeH);
		ofSetColor(active ? ofColor(255, 45, 220, 180) : ofColor(90, 50, 90, 120));
		ofDrawRectangle(x + lifeW - 5.0f, lifeY, 5.0f, lifeH);
	}

	const float barX = panelX + 18.0f;
	const float barY = panelY + 140.0f;
	const float barW = panelW - 36.0f;
	const float barH = 12.0f;
	ofSetColor(255, 255, 255, 42);
	ofDrawRectangle(barX, barY, barW, barH);
	ofSetColor(powerCharge >= 100.0f ? ofColor(255, 55, 220, 230) : ofColor(75, 210, 255, 200));
	ofDrawRectangle(barX, barY, barW * ofClamp(powerCharge / 100.0f, 0.0f, 1.0f), barH);
	ofNoFill();
	ofSetColor(80, 235, 255, 160);
	ofDrawRectangle(barX, barY, barW, barH);
	ofFill();
	ofSetColor(255);
	ofDrawBitmapString(powerCharge >= 100.0f ? "PODER // SPACE PRONTO" : "PODER // " + ofToString((int)powerCharge) + "%", barX, barY + 31);

	const float effectW = 254.0f;
	const float effectH = 116.0f;
	const float effectX = ofGetWidth() - effectW - 18.0f;
	const float effectY = 18.0f;
	ofSetColor(4, 0, 22, 145);
	ofDrawRectangle(effectX, effectY, effectW, effectH);
	ofSetColor(255, 45, 220, 32);
	ofDrawRectangle(effectX + effectW - 7.0f, effectY, 7.0f, effectH);
	ofNoFill();
	ofSetColor(55, 235, 255, 140);
	ofDrawRectangle(effectX, effectY, effectW, effectH);
	ofSetColor(255, 45, 220, 120 * pulse);
	ofDrawLine(effectX + 18.0f, effectY - 5.0f, effectX + effectW - 18.0f, effectY - 5.0f);
	ofDrawLine(effectX + 18.0f, effectY + effectH + 5.0f, effectX + effectW - 18.0f, effectY + effectH + 5.0f);
	ofFill();
	ofSetColor(255);
	ofDrawBitmapString("POWER UPS", effectX + 14.0f, effectY + 22.0f);
	ofSetColor(speedBoostTimer > 0.0f ? ofColor(65, 235, 255) : ofColor(70, 75, 95));
	ofDrawBitmapString(string("BOOST ") + ofToString(speedBoostTimer, 1) + "s", effectX + 14.0f, effectY + 47.0f);
	ofSetColor(shieldTimer > 0.0f ? ofColor(75, 255, 130) : ofColor(70, 75, 95));
	ofDrawBitmapString(string("ESCUDO ") + ofToString(shieldTimer, 1) + "s", effectX + 118.0f, effectY + 47.0f);
	ofSetColor(heavyShotTimer > 0.0f ? ofColor(255, 155, 45) : ofColor(70, 75, 95));
	ofDrawBitmapString(string("TIRO x2 ") + ofToString(heavyShotTimer, 1) + "s", effectX + 14.0f, effectY + 70.0f);
	ofSetColor(powerCharge >= 100.0f ? ofColor(255, 230, 70) : ofColor(70, 75, 95));
	ofDrawBitmapString(powerCharge >= 100.0f ? "ESPECIAL PRONTO" : "ESPECIAL CARREGANDO", effectX + 14.0f, effectY + 94.0f);

	if (powerUpNoticeTimer > 0.0f) {
		const float alpha = ofMap(powerUpNoticeTimer, 0.0f, 2.2f, 0.0f, 255.0f, true);
		ofSetColor(255, 230, 90, alpha);
		ofDrawBitmapStringHighlight(lastPowerUpLabel, effectX + 8.0f, effectY + effectH + 35.0f, ofColor(0, 0, 0, 135), ofColor(255, 230, 90, alpha));
	}

	drawBossHud();

	if (waveMessageTimer > 0.0f) {
		ofSetColor(255, 230, 120, ofMap(waveMessageTimer, 0.0f, 1.8f, 0.0f, 255.0f, true));
		ofDrawBitmapStringHighlight("ONDA " + ofToString(wave), panelX + 18.0f, panelY + panelH + 54.0f, ofColor(0, 0, 0, 130), ofColor(255, 230, 120));
	}

	if (hasHeavyMeteor) {
		const float blink = ofMap(sinf(ofGetElapsedTimef() * 8.0f), -1.0f, 1.0f, 0.35f, 1.0f);
		const float warningW = 310.0f;
		const float warningX = ofGetWidth() - warningW - 18.0f;
		const float warningY = effectY + effectH + 58.0f;
		ofNoFill();
		ofSetLineWidth(2.0f);
		ofSetColor(255, 35, 30, 180 * blink);
		ofDrawRectangle(warningX, warningY, warningW, 36.0f);
		ofSetColor(255, 65, 220, 150 * blink);
		ofDrawLine(warningX - 22.0f, warningY + 18.0f, warningX - 4.0f, warningY + 18.0f);
		ofDrawLine(warningX + warningW + 4.0f, warningY + 18.0f, warningX + warningW + 22.0f, warningY + 18.0f);
		ofFill();
		ofSetColor(80, 0, 0, 115);
		ofDrawRectangle(warningX, warningY, warningW * ofLerp(0.35f, 1.0f, closestHeavyDepth), 36.0f);
		ofSetColor(255, 70, 45, 255 * blink);
		ofDrawBitmapString("ALERTA // METEORO PESADO", warningX + 46.0f, warningY + 23.0f);
	}

	ofPopStyle();
}

void ofApp::drawBossWarningOverlay() {
	if (!bossWarningActive) {
		return;
	}

	const float w = ofGetWidth();
	const float h = ofGetHeight();
	const float elapsed = 2.8f - bossWarningTimer;
	const float flash = ofMap(sinf(ofGetElapsedTimef() * 18.0f), -1.0f, 1.0f, 0.35f, 1.0f);
	const float progress = ofClamp(elapsed / 2.8f, 0.0f, 1.0f);
	const float gate = ofMap(sinf(ofGetElapsedTimef() * 9.0f), -1.0f, 1.0f, 0.0f, 1.0f);

	ofPushStyle();
	ofDisableDepthTest();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(95, 0, 0, 190 + 45 * flash);
	ofDrawRectangle(0, 0, w, h);
	ofSetColor(255, 0, 0, 58 * flash);
	ofDrawRectangle(0, 0, w, h);

	ofNoFill();
	ofSetLineWidth(8.0f);
	ofSetColor(255, 30, 28, 235 * flash);
	ofDrawRectangle(34.0f, 34.0f, w - 68.0f, h - 68.0f);
	ofSetLineWidth(2.0f);
	ofSetColor(255, 220, 90, 180 * flash);
	ofDrawRectangle(58.0f, 58.0f, w - 116.0f, h - 116.0f);

	for (int i = 0; i < 18; ++i) {
		const float y = fmod(ofGetElapsedTimef() * 120.0f + i * 52.0f, h);
		ofSetColor(255, 40, 35, (i % 2 == 0 ? 45 : 24) * flash);
		ofDrawLine(0, y, w, y);
	}

	ofEnableBlendMode(OF_BLENDMODE_ADD);
	for (int i = 0; i < 10; ++i) {
		const float y = h * 0.5f - 170.0f + i * 34.0f + sinf(ofGetElapsedTimef() * 7.0f + i) * 6.0f;
		const float bandW = ofRandom(w * 0.08f, w * 0.32f);
		const float x = (i % 2 == 0) ? ofRandom(20.0f, w * 0.22f) : ofRandom(w * 0.64f, w - bandW - 20.0f);
		ofSetColor(i % 2 == 0 ? ofColor(255, 25, 215, 70 * flash) : ofColor(45, 235, 255, 62 * flash));
		ofDrawRectangle(x, y, bandW, ofRandom(4.0f, 12.0f));
	}

	ofSetLineWidth(2.0f);
	for (int i = 0; i < 5; ++i) {
		const float x = w * (0.12f + i * 0.19f);
		ofSetColor(255, 35, 45, 48 * flash);
		ofDrawLine(x, 64.0f, x + sinf(ofGetElapsedTimef() * 4.0f + i) * 26.0f, h - 64.0f);
	}

	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(255, 230, 90, 210 * flash);
	ofDrawBitmapString("/// SYSTEM BREACH ///", 70.0f, 92.0f);
	ofDrawBitmapString("HOSTILE SIGNATURE LOCKED", w - 270.0f, h - 82.0f);
	ofSetColor(80, 235, 255, 170 * flash);
	ofDrawBitmapString("SECTOR OVERRIDE  //  WARNING LEVEL MAX", 70.0f, h - 82.0f);
	ofDrawBitmapString("CORE ALERT", w - 150.0f, 92.0f);

	const float barH = ofLerp(18.0f, 110.0f, gate);
	ofFill();
	ofSetColor(255, 0, 0, 85 * flash);
	ofDrawRectangle(0, h * 0.5f - barH * 0.5f, w, barH);

	const string label = "ALERTA";
	if (warningTitleFontLoaded) {
		const ofRectangle bounds = warningTitleFont.getStringBoundingBox(label, 0.0f, 0.0f);
		const float targetW = w * 0.88f;
		const float targetH = h * 0.36f;
		const float textScale = std::min(targetW / bounds.width, targetH / bounds.height) * (0.98f + flash * 0.05f);
		const float textX = w * 0.5f - bounds.width * textScale * 0.5f - bounds.x * textScale;
		const float textY = h * 0.5f - 38.0f - bounds.height * textScale * 0.5f - bounds.y * textScale;

		ofPushMatrix();
		ofTranslate(textX + ofRandom(-8.0f, 8.0f) * flash, textY + ofRandom(-5.0f, 5.0f) * flash);
		ofScale(textScale, textScale);
		ofSetColor(0, 0, 0, 235);
		warningTitleFont.drawString(label, 8.0f, 10.0f);
		ofSetColor(255, 0, 0, 180 * flash);
		warningTitleFont.drawString(label, -5.0f, 2.0f);
		ofSetColor(255, 30, 220, 115 * flash);
		warningTitleFont.drawString(label, ofRandom(-18.0f, -8.0f) * flash, ofRandom(-5.0f, 5.0f) * flash);
		ofSetColor(80, 235, 255, 125 * flash);
		warningTitleFont.drawString(label, 5.0f, -2.0f);
		ofSetColor(60, 240, 255, 95 * flash);
		warningTitleFont.drawString(label, ofRandom(8.0f, 20.0f) * flash, ofRandom(-6.0f, 6.0f) * flash);
		ofSetColor(255, 245, 210, 255);
		warningTitleFont.drawString(label, 0.0f, 0.0f);
		for (int i = 0; i < 7; ++i) {
			const float sliceY = bounds.y + ofRandom(bounds.height * 0.08f, bounds.height * 0.88f);
			ofSetColor(i % 2 == 0 ? ofColor(255, 30, 220, 95 * flash) : ofColor(70, 235, 255, 95 * flash));
			ofDrawRectangle(bounds.x + ofRandom(-18.0f, 22.0f), sliceY, bounds.width * ofRandom(0.18f, 0.55f), ofRandom(3.0f, 8.0f));
		}
		ofPopMatrix();
	} else {
		const float textScale = (w * 0.88f) / (label.size() * 8.0f);
		ofPushMatrix();
		ofTranslate(w * 0.06f, h * 0.5f - 38.0f);
		ofScale(textScale, textScale);
		ofSetColor(255, 245, 210, 255);
		ofDrawBitmapString(label, 0.0f, 0.0f);
		ofPopMatrix();
	}

	const string warningLabel = "CUIDADO";
	if (warningSubtitleFontLoaded) {
		const ofRectangle bounds = warningSubtitleFont.getStringBoundingBox(warningLabel, 0.0f, 0.0f);
		const float targetW = w * 0.62f;
		const float textScale = (targetW / bounds.width) * (0.98f + flash * 0.04f);
		const float textX = w * 0.5f - bounds.width * textScale * 0.5f - bounds.x * textScale;
		const float textY = h * 0.5f + 92.0f - bounds.height * textScale * 0.5f - bounds.y * textScale;
		ofPushMatrix();
		ofTranslate(textX, textY);
		ofScale(textScale, textScale);
		ofSetColor(0, 0, 0, 220);
		warningSubtitleFont.drawString(warningLabel, 5.0f, 6.0f);
		ofSetColor(255, 40, 220, 120 * flash);
		warningSubtitleFont.drawString(warningLabel, -3.0f, 0.0f);
		ofSetColor(70, 235, 255, 120 * flash);
		warningSubtitleFont.drawString(warningLabel, 3.0f, -1.0f);
		ofSetColor(255, 230, 90, 255);
		warningSubtitleFont.drawString(warningLabel, 0.0f, 0.0f);
		ofPopMatrix();
	} else {
		const float warningScale = (w * 0.62f) / (warningLabel.size() * 8.0f);
		ofPushMatrix();
		ofTranslate(w * 0.19f, h * 0.5f + 92.0f);
		ofScale(warningScale, warningScale);
		ofSetColor(255, 230, 90, 255);
		ofDrawBitmapString(warningLabel, 0.0f, 0.0f);
		ofPopMatrix();
	}

	ofSetColor(255, 230, 90, 240 * flash);
	ofDrawBitmapStringHighlight("ENTIDADE HOSTIL ENTRANDO NO SETOR", w * 0.5f - 142.0f, h * 0.5f + 184.0f, ofColor(30, 0, 0, 180), ofColor(255, 230, 90, 240 * flash));
	ofSetColor(255, 255, 255, 230);
	ofDrawBitmapString("BOSS EM " + ofToString(std::max(0.0f, bossWarningTimer), 1) + "s", w * 0.5f - 40.0f, h * 0.5f + 216.0f);

	ofNoFill();
	ofSetLineWidth(3.0f);
	for (int i = 0; i < 6; ++i) {
		const float r = ofLerp(80.0f, 430.0f, fmod(progress + i * 0.16f, 1.0f));
		ofSetColor(255, 230, 90, ofMap(r, 80.0f, 430.0f, 180.0f, 0.0f, true) * flash);
		ofDrawCircle(w * 0.5f, h * 0.5f, r);
	}
	ofPopStyle();
}

void ofApp::drawBossDeathGlitchOverlay() {
	if (bossDeathGlitchTimer <= 0.0f) {
		return;
	}

	const float w = ofGetWidth();
	const float h = ofGetHeight();
	const float progress = 1.0f - ofClamp(bossDeathGlitchTimer / 2.15f, 0.0f, 1.0f);
	const float flash = ofMap(sinf(ofGetElapsedTimef() * 34.0f), -1.0f, 1.0f, 0.25f, 1.0f);
	const float collapse = progress * progress;

	ofPushStyle();
	ofDisableDepthTest();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(0, 0, 0, 80 + 130 * collapse);
	ofDrawRectangle(0, 0, w, h);
	ofSetColor(255, 0, 45, 60 * flash);
	ofDrawRectangle(0, 0, w, h);
	ofSetColor(30, 235, 255, 36 * (1.0f - collapse));
	ofDrawRectangle(0, 0, w, h);

	ofEnableBlendMode(OF_BLENDMODE_ADD);
	for (int i = 0; i < 34; ++i) {
		const float y = ofRandom(h);
		const float x = ofRandom(-w * 0.12f, w * 0.92f);
		const float bandW = ofRandom(w * 0.12f, w * 0.72f);
		const float bandH = ofRandom(4.0f, 24.0f);
		ofSetColor(i % 2 == 0 ? ofColor(255, 35, 220, 95 * flash) : ofColor(45, 235, 255, 85 * flash));
		ofDrawRectangle(x + ofRandom(-32.0f, 32.0f), y, bandW, bandH);
	}

	ofSetLineWidth(2.0f);
	for (int i = 0; i < 24; ++i) {
		const float y = i * (h / 24.0f) + sinf(ofGetElapsedTimef() * 14.0f + i) * 9.0f;
		ofSetColor(255, 255, 255, (i % 3 == 0 ? 58 : 22) * flash);
		ofDrawLine(0, y, w, y);
	}

	const string label = "SIGNAL LOST";
	if (warningTitleFontLoaded) {
		const ofRectangle bounds = warningTitleFont.getStringBoundingBox(label, 0.0f, 0.0f);
		const float textScale = std::min((w * 0.84f) / bounds.width, (h * 0.28f) / bounds.height);
		const float textX = w * 0.5f - bounds.width * textScale * 0.5f - bounds.x * textScale;
		const float textY = h * 0.5f - bounds.height * textScale * 0.5f - bounds.y * textScale;
		ofPushMatrix();
		ofTranslate(textX + ofRandom(-18.0f, 18.0f) * flash, textY + ofRandom(-8.0f, 8.0f) * flash);
		ofScale(textScale, textScale);
		ofSetColor(0, 0, 0, 230);
		warningTitleFont.drawString(label, 8.0f, 10.0f);
		ofSetColor(255, 35, 220, 170 * flash);
		warningTitleFont.drawString(label, ofRandom(-18.0f, -7.0f), ofRandom(-4.0f, 5.0f));
		ofSetColor(70, 235, 255, 170 * flash);
		warningTitleFont.drawString(label, ofRandom(7.0f, 18.0f), ofRandom(-5.0f, 4.0f));
		ofSetColor(255, 245, 210, 245);
		warningTitleFont.drawString(label, 0.0f, 0.0f);
		ofPopMatrix();
	} else {
		ofSetColor(255, 245, 210, 245);
		ofDrawBitmapStringHighlight(label, w * 0.5f - 48.0f, h * 0.5f, ofColor(0, 0, 0, 180), ofColor(255, 245, 210));
	}

	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(255, 230, 90, 235 * flash);
	ofDrawBitmapStringHighlight("BOSS CORE DELETED  //  RESTORING SIGNAL", w * 0.5f - 152.0f, h * 0.5f + 118.0f, ofColor(0, 0, 0, 170), ofColor(255, 230, 90, 235 * flash));
	ofSetColor(80, 235, 255, 195);
	ofDrawBitmapString("VICTORY SCREEN IN " + ofToString(std::max(0.0f, bossDeathGlitchTimer), 1) + "s", w * 0.5f - 76.0f, h * 0.5f + 150.0f);
	ofPopStyle();
}

void ofApp::drawBossHud() {
	if (!bossActive) {
		return;
	}

	const float w = 460.0f;
	const float h = 70.0f;
	const float x = ofGetWidth() - w - 18.0f;
	const float y = ofGetHeight() - h - 18.0f;
	const float health01 = ofClamp(bossHealth / (float)maxBossHealth, 0.0f, 1.0f);
	const float blink = ofMap(sinf(ofGetElapsedTimef() * 8.0f), -1.0f, 1.0f, 0.45f, 1.0f);

	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(4, 0, 22, 172);
	ofDrawRectangle(x, y, w, h);
	ofSetColor(255, 45, 220, 35);
	ofDrawRectangle(x + w - 8.0f, y, 8.0f, h);
	ofNoFill();
	ofSetLineWidth(2.5f);
	ofSetColor(bossVulnerable ? ofColor(255, 230, 70, 230 * blink) : ofColor(255, 45, 220, 210));
	ofDrawRectangle(x, y, w, h);
	ofSetLineWidth(4.0f);
	ofDrawLine(x - 3.0f, y + 26.0f, x + 34.0f, y - 3.0f);
	ofDrawLine(x + w - 34.0f, y + h + 3.0f, x + w + 3.0f, y + h - 26.0f);
	ofFill();

	ofSetColor(255);
	ofDrawBitmapString(bossVulnerable ? "BOSS // VULNERAVEL - MIRE NO NUCLEO" : "BOSS // ATAQUE - DESTRUA AS ESFERAS", x + 18.0f, y + 21.0f);
	if (!bossVulnerable) {
		ofSetColor(255, 230, 90);
		ofDrawBitmapString("ESFERAS " + ofToString(bossOrbsDestroyedThisPhase) + "/" + ofToString(bossOrbsToOpen), x + w - 132.0f, y + 21.0f);
	}
	ofSetColor(60, 20, 55, 220);
	ofDrawRectangle(x + 18.0f, y + 40.0f, w - 36.0f, 14.0f);
	ofSetColor(bossVulnerable ? ofColor(255, 230, 70, 230) : ofColor(255, 50, 95, 230));
	ofDrawRectangle(x + 18.0f, y + 40.0f, (w - 36.0f) * health01, 14.0f);
	ofSetColor(80, 235, 255, 175);
	for (int i = 0; i <= maxBossHealth; ++i) {
		const float tickX = x + 18.0f + (w - 36.0f) * (i / (float)maxBossHealth);
		ofDrawLine(tickX, y + 37.0f, tickX, y + 58.0f);
	}
	ofPopStyle();
}

void ofApp::updateParticles(float dt) {
	for (auto& particle : particles) {
		particle.age += dt;
		particle.velocity *= 0.985f;
		particle.position += particle.velocity * dt;
	}
	particles.erase(
		remove_if(particles.begin(), particles.end(), [](const Particle& particle) {
			return particle.age >= particle.lifetime;
		}),
		particles.end());
}

void ofApp::startBossWarning() {
	bossWarningActive = true;
	bossWarningTimer = 2.8f;
	bossWarningSoundTimer = 0.0f;
	screenShake = 0.75f;
	waveMessageTimer = 0.0f;
	meteors.clear();
	bossOrbs.clear();
	bossTargets.clear();
	triggerSound(4, 1.4f, 0.5f);
}

void ofApp::startBossFight() {
	bossActive = true;
	bossWarningActive = false;
	bossVulnerable = false;
	bossHealth = maxBossHealth;
	bossTimer = 0.0f;
	bossDwell = 0.0f;
	bossOrbsDestroyedThisPhase = 0;
	bossOrbCooldown = 1.45f;
	bossTargetCooldown = 1.4f;
	bossWarningTimer = 0.0f;
	bossWarningSoundTimer = 0.0f;
	bossIntroTimer = 2.2f;
	bossOrbs.clear();
	bossTargets.clear();
	meteors.clear();
	destroyedInWave = 0;
	waveMessageTimer = 2.0f;
	screenShake = 1.0f;
	spawnExplosionParticles(bossWorldPosition(), ofColor(255, 45, 95), 64);
	triggerSound(3, 1.3f, 0.5f);
}

void ofApp::spawnBossOrb() {
	BossOrb orb;
	const float side = ofRandom(-1.0f, 1.0f);
	orb.position.set(side * ofRandom(35.0f, 145.0f), ofRandom(-120.0f, 75.0f), -690.0f);
	const ofVec3f target(ofRandom(-360.0f, 360.0f), ofRandom(-240.0f, 250.0f), nearZ + 90.0f);
	orb.velocity = (target - orb.position).getNormalized() * ofRandom(95.0f, 155.0f);
	orb.radius = ofRandom(12.0f, 21.0f);
	orb.phase = ofRandom(TWO_PI);
	orb.color = ofColor::fromHsb((int)ofRandom(215, 245), 210, 255);
	bossOrbs.push_back(orb);
	triggerSound(0, 0.72f, ofMap(orb.position.x, -360.0f, 360.0f, 0.12f, 0.88f, true));
}

void ofApp::spawnBossTarget() {
	BossTarget target;
	const float margin = 95.0f;
	target.position.set(ofRandom(margin, ofGetWidth() - margin), ofRandom(margin + 40.0f, ofGetHeight() - margin));
	target.radius = ofRandom(46.0f, 62.0f);
	target.duration = ofRandom(2.25f, 3.05f);
	target.phase = ofRandom(TWO_PI);
	bossTargets.push_back(target);
	triggerSound(3, 0.55f, ofMap(target.position.x, 0, ofGetWidth(), 0.1f, 0.9f, true));
}

void ofApp::spawnPowerUp(bool startFar) {
	PowerUp powerUp;
	const float roll = ofRandom(1.0f);
	if (roll < 0.24f) {
		powerUp.type = 0;
		powerUp.color = ofColor(65, 235, 255);
	} else if (roll < 0.46f) {
		powerUp.type = 1;
		powerUp.color = ofColor(75, 255, 130);
	} else if (roll < 0.72f) {
		powerUp.type = 2;
		powerUp.color = ofColor(255, 80, 220);
	} else if (roll < 0.88f) {
		powerUp.type = 3;
		powerUp.color = ofColor(255, 150, 45);
	} else {
		powerUp.type = 4;
		powerUp.color = ofColor(255, 230, 70);
	}

	const float z = startFar ? ofRandom(farZ * 0.92f, -1050.0f) : ofRandom(-1250.0f, 160.0f);
	const float depth01 = ofMap(z, farZ, nearZ, 0.0f, 1.0f, true);
	const float spreadX = ofLerp(240.0f, 620.0f, depth01);
	const float spreadY = ofLerp(120.0f, 420.0f, depth01);
	powerUp.position.set(ofRandom(-spreadX, spreadX), ofRandom(-spreadY, spreadY), z);
	powerUp.drift.set(ofRandom(-26.0f, 26.0f), ofRandom(-18.0f, 18.0f), 0.0f);
	powerUp.radius = ofRandom(24.0f, 36.0f);
	powerUp.speed = ofRandom(86.0f, 138.0f);
	powerUp.phase = ofRandom(TWO_PI);
	powerUps.push_back(powerUp);
}

void ofApp::updateBoss(float dt) {
	const float attackDuration = 9.0f;
	const float vulnerableDuration = 4.4f;

	bossTimer += dt;
	if (bossIntroTimer > 0.0f) {
		bossOrbCooldown = std::max(bossOrbCooldown, 0.45f);
		bossTargetCooldown = std::max(bossTargetCooldown, 0.45f);
		return;
	}
	bossOrbCooldown -= dt;
	updateBossTargets(dt);

	for (auto& orb : bossOrbs) {
		if (orb.exploding) {
			orb.explosion += dt;
			continue;
		}

		orb.position += orb.velocity * dt;
		orb.position.x += sinf(ofGetElapsedTimef() * 1.8f + orb.phase) * 10.0f * dt;
		orb.position.y += cosf(ofGetElapsedTimef() * 1.55f + orb.phase) * 7.0f * dt;

		const glm::vec3 screen3 = camera.worldToScreen(glm::vec3(orb.position.x, orb.position.y, orb.position.z));
		const ofVec2f screen(screen3.x, screen3.y);
		const float depth01 = ofMap(orb.position.z, -700.0f, nearZ, 0.0f, 1.0f, true);
		const float hitRadius = ofLerp(38.0f, 82.0f, depth01);
		if (screen.distance(aimPosition) <= hitRadius) {
			orb.dwell += dt;
			if (orb.dwell >= 0.34f) {
				launchMissileAt(screen, orb.color);
				spawnExplosionParticles(orb.position, orb.color, 14);
				orb.exploding = true;
				orb.explosion = 0.0f;
				if (!bossVulnerable) {
					bossOrbsDestroyedThisPhase++;
				}
				score += 40 + combo * 4;
				combo++;
				powerCharge = ofClamp(powerCharge + 8.0f, 0.0f, 100.0f);
				triggerSound(1, 0.9f, ofMap(screen.x, 0, ofGetWidth(), 0.1f, 0.9f, true));
			}
		} else {
			orb.dwell = ofClamp(orb.dwell - dt * 1.7f, 0.0f, 0.34f);
		}

		if (orb.position.z > nearZ && !orb.exploding) {
			if (shieldTimer > 0.0f) {
				shieldTimer = std::max(0.0f, shieldTimer - 2.0f);
			} else {
				lives = std::max(0, lives - 1);
			}
			combo = 0;
			damageFlash = 1.0f;
			screenShake = 1.0f;
			orb.exploding = true;
			orb.explosion = 0.0f;
			triggerSound(2, 0.95f, ofMap(screen.x, 0, ofGetWidth(), 0.1f, 0.9f, true));
		}
	}

	bossOrbs.erase(
		remove_if(bossOrbs.begin(), bossOrbs.end(), [](const BossOrb& orb) {
			return orb.exploding && orb.explosion > 0.35f;
		}),
		bossOrbs.end());

	if (lives <= 0) {
		gameStarted = false;
		gameWon = false;
		bossActive = false;
		bossWarningActive = false;
		bossVulnerable = false;
		damageFlash = 0.0f;
		screenShake = 0.0f;
		shieldTimer = 0.0f;
		speedBoostTimer = 0.0f;
		heavyShotTimer = 0.0f;
		missiles.clear();
		bossOrbs.clear();
		bossTargets.clear();
		powerUps.clear();
		triggerSound(3, 1.35f, 0.5f);
		return;
	}

	if (bossVulnerable) {
		const ofVec2f bossScreen = bossScreenPosition();
		if (bossScreen.distance(aimPosition) <= 150.0f) {
			bossDwell += dt;
			if (bossDwell >= 0.72f) {
				damageBoss();
				bossDwell = 0.0f;
			}
		} else {
			bossDwell = ofClamp(bossDwell - dt * 1.5f, 0.0f, 0.72f);
		}

		if (bossTimer >= vulnerableDuration && bossActive) {
			bossVulnerable = false;
			bossTimer = 0.0f;
			bossDwell = 0.0f;
			bossOrbsDestroyedThisPhase = 0;
			bossOrbCooldown = 0.8f;
			triggerSound(3, 0.85f, 0.5f);
		}
	} else {
		bossDwell = 0.0f;
		if (bossOrbCooldown <= 0.0f) {
			spawnBossOrb();
			if (ofRandom(1.0f) < 0.28f) {
				spawnBossOrb();
			}
			bossOrbCooldown = ofRandom(0.72f, 1.05f);
		}
		const bool destroyedEnoughOrbs = bossOrbsDestroyedThisPhase >= bossOrbsToOpen && bossTimer >= 5.0f;
		const bool failsafeWindow = bossTimer >= attackDuration + 7.0f;
		if (destroyedEnoughOrbs || failsafeWindow) {
			bossVulnerable = true;
			bossTimer = 0.0f;
			bossOrbCooldown = 1.4f;
			triggerSound(3, 1.05f, 0.5f);
		}
	}
}

void ofApp::damageBoss() {
	if (!bossActive || !bossVulnerable) {
		return;
	}

	const ofVec3f bossPosition = bossWorldPosition();
	const int shotDamage = heavyShotTimer > 0.0f ? 2 : 1;
	bossHealth = std::max(0, bossHealth - shotDamage);
	score += (300 + combo * 35) * shotDamage;
	combo++;
	powerCharge = ofClamp(powerCharge + 18.0f * shotDamage, 0.0f, 100.0f);
	screenShake = 0.7f;
	launchMissileAt(bossScreenPosition(), ofColor(255, 230, 70));
	if (heavyShotTimer > 0.0f) {
		launchMissileAt(bossScreenPosition() + ofVec2f(24.0f, -18.0f), ofColor(255, 155, 45));
	}
	spawnExplosionParticles(bossPosition, ofColor(255, 230, 70), heavyShotTimer > 0.0f ? 38 : 26);
	triggerSound(2, 1.25f, 0.5f);

	if (bossHealth <= 0) {
		score += 2000;
		gameWon = false;
		victoryScreenTimer = 0.0f;
		bossDeathGlitchTimer = 2.15f;
		bossActive = false;
		bossWarningActive = false;
		bossVulnerable = false;
		screenShake = 1.0f;
		damageFlash = 0.5f;
		shieldTimer = 0.0f;
		speedBoostTimer = 0.0f;
		heavyShotTimer = 0.0f;
		bossOrbs.clear();
		bossTargets.clear();
		powerUps.clear();
		missiles.clear();
		triggerSound(3, 1.6f, 0.5f);
	}
}

void ofApp::updateBossTargets(float dt) {
	if (!bossActive) {
		return;
	}

	if (!bossVulnerable) {
		bossTargetCooldown -= dt;
		if (bossTargetCooldown <= 0.0f && bossTargets.size() < 3) {
			spawnBossTarget();
			if (ofRandom(1.0f) < 0.14f && bossTargets.size() < 3) {
				spawnBossTarget();
			}
			bossTargetCooldown = ofRandom(1.85f, 2.8f);
		}
	}

	for (auto& target : bossTargets) {
		if (target.resolved) {
			continue;
		}

		target.age += dt;
		if (target.position.distance(aimPosition) <= target.radius) {
			target.resolved = true;
			score += 90 + combo * 5;
			combo++;
			powerCharge = ofClamp(powerCharge + 9.0f, 0.0f, 100.0f);
			powerPulse = std::max(powerPulse, 0.45f);
			triggerSound(1, 0.85f, ofMap(target.position.x, 0, ofGetWidth(), 0.1f, 0.9f, true));
			continue;
		}

		if (target.age >= target.duration) {
			target.resolved = true;
			if (shieldTimer > 0.0f) {
				shieldTimer = std::max(0.0f, shieldTimer - 2.5f);
			} else {
				lives = std::max(0, lives - 1);
			}
			combo = 0;
			damageFlash = 1.0f;
			screenShake = 1.0f;
			triggerSound(2, 0.9f, ofMap(target.position.x, 0, ofGetWidth(), 0.1f, 0.9f, true));
		}
	}

	bossTargets.erase(
		remove_if(bossTargets.begin(), bossTargets.end(), [](const BossTarget& target) {
			return target.resolved;
		}),
		bossTargets.end());
}

void ofApp::updatePowerUps(float dt) {
	powerUpSpawnTimer -= dt;
	const size_t maxPowerUps = bossActive ? 6 : 5;
	if (powerUpSpawnTimer <= 0.0f && powerUps.size() < maxPowerUps) {
		spawnPowerUp(true);
		powerUpSpawnTimer = ofRandom(bossActive ? 2.2f : 3.0f, bossActive ? 3.8f : 4.8f);
	}

	for (auto& powerUp : powerUps) {
		if (powerUp.collected) {
			continue;
		}

		const float depth01 = ofMap(powerUp.position.z, farZ, nearZ, 0.0f, 1.0f, true);
		powerUp.position.z += powerUp.speed * ofLerp(0.82f, 1.55f, depth01) * dt;
		powerUp.position.x += (powerUp.drift.x + sinf(ofGetElapsedTimef() * 1.7f + powerUp.phase) * 18.0f) * dt;
		powerUp.position.y += (powerUp.drift.y + cosf(ofGetElapsedTimef() * 1.45f + powerUp.phase) * 12.0f) * dt;

		const glm::vec3 screen3 = camera.worldToScreen(glm::vec3(powerUp.position.x, powerUp.position.y, powerUp.position.z));
		const ofVec2f screen(screen3.x, screen3.y);
		const float hitRadius = ofLerp(46.0f, 90.0f, depth01);
		const float neededDwell = speedBoostTimer > 0.0f ? 0.16f : 0.26f;
		if (screen.distance(aimPosition) <= hitRadius) {
			powerUp.dwell += dt;
			if (powerUp.dwell >= neededDwell) {
				collectPowerUp(powerUp);
			}
		} else {
			powerUp.dwell = ofClamp(powerUp.dwell - dt * 1.9f, 0.0f, neededDwell);
		}
	}

	powerUps.erase(
		remove_if(powerUps.begin(), powerUps.end(), [this](const PowerUp& powerUp) {
			return powerUp.collected || powerUp.position.z > nearZ + 180.0f;
		}),
		powerUps.end());
}

void ofApp::collectPowerUp(PowerUp& powerUp) {
	if (powerUp.collected) {
		return;
	}

	powerUp.collected = true;
	const glm::vec3 screen3 = camera.worldToScreen(glm::vec3(powerUp.position.x, powerUp.position.y, powerUp.position.z));
	const ofVec2f screen(screen3.x, screen3.y);
	launchMissileAt(screen, powerUp.color);
	spawnExplosionParticles(powerUp.position, powerUp.color, 20);
	score += 75;
	powerCharge = ofClamp(powerCharge + 10.0f, 0.0f, 100.0f);
	powerPulse = 0.65f;
	screenShake = std::max(screenShake, 0.2f);

	if (powerUp.type == 0) {
		speedBoostTimer = 10.0f;
		lastPowerUpLabel = "BOOST // LOCK RAPIDO";
	} else if (powerUp.type == 1) {
		shieldTimer = 12.0f;
		lastPowerUpLabel = "ESCUDO // NAVE PROTEGIDA";
	} else if (powerUp.type == 2) {
		lives = std::min(maxLives, lives + 4);
		lastPowerUpLabel = "VIDA EXTRA // +4";
	} else if (powerUp.type == 3) {
		heavyShotTimer = 9.0f;
		lastPowerUpLabel = "TIRO PESADO // DANO x2";
	} else {
		powerCharge = 100.0f;
		powerPulse = 1.0f;
		lastPowerUpLabel = "PODER MAX // SPACE PRONTO";
	}

	powerUpNoticeTimer = 2.2f;
	triggerSound(3, 1.1f, ofMap(screen.x, 0, ofGetWidth(), 0.1f, 0.9f, true));
}

void ofApp::updateAimAssist(float dt) {
	hasAssistedTarget = false;
	assistedTarget = mousePosition;

	float bestDistance = aimAssistRadius;
	for (const auto& meteor : meteors) {
		if (meteor.exploding) {
			continue;
		}

		const ofVec2f screen = screenPositionFor(meteor);
		const float distance = screen.distance(mousePosition);
		if (distance < bestDistance) {
			bestDistance = distance;
			assistedTarget = screen;
			hasAssistedTarget = true;
		}
	}

	for (const auto& powerUp : powerUps) {
		if (powerUp.collected) {
			continue;
		}
		const glm::vec3 screen3 = camera.worldToScreen(glm::vec3(powerUp.position.x, powerUp.position.y, powerUp.position.z));
		const ofVec2f screen(screen3.x, screen3.y);
		const float distance = screen.distance(mousePosition);
		if (distance < bestDistance) {
			bestDistance = distance;
			assistedTarget = screen;
			hasAssistedTarget = true;
		}
	}

	if (bossActive) {
		if (bossVulnerable) {
			const ofVec2f screen = bossScreenPosition();
			const float distance = screen.distance(mousePosition);
			if (distance < bestDistance) {
				bestDistance = distance;
				assistedTarget = screen;
				hasAssistedTarget = true;
			}
		}

		for (const auto& orb : bossOrbs) {
			if (orb.exploding) {
				continue;
			}
			const glm::vec3 screen3 = camera.worldToScreen(glm::vec3(orb.position.x, orb.position.y, orb.position.z));
			const ofVec2f screen(screen3.x, screen3.y);
			const float distance = screen.distance(mousePosition);
			if (distance < bestDistance) {
				bestDistance = distance;
				assistedTarget = screen;
				hasAssistedTarget = true;
			}
		}
	}

	ofVec2f desired = mousePosition;
	if (hasAssistedTarget) {
		const float closeness = 1.0f - ofClamp(bestDistance / aimAssistRadius, 0.0f, 1.0f);
		desired = mousePosition.getInterpolated(assistedTarget, aimAssistStrength * closeness);
	}

	const float k = 1.0f - expf(-18.0f * dt);
	aimPosition.interpolate(desired, ofClamp(k, 0.0f, 1.0f));
	waveMessageTimer = ofClamp(waveMessageTimer - dt, 0.0f, 2.0f);
}

float ofApp::currentDwellProgress() const {
	float best = 0.0f;
	for (const auto& meteor : meteors) {
		if (!meteor.exploding && isMouseOver(meteor)) {
			const float activeDwellToExplode = speedBoostTimer > 0.0f ? dwellToExplode * 0.58f : dwellToExplode;
			best = max(best, meteor.dwell / activeDwellToExplode);
		}
	}
	for (const auto& powerUp : powerUps) {
		if (!powerUp.collected) {
			best = max(best, powerUp.dwell / (speedBoostTimer > 0.0f ? 0.16f : 0.26f));
		}
	}
	if (bossActive) {
		for (const auto& orb : bossOrbs) {
			if (!orb.exploding) {
				best = max(best, orb.dwell / 0.34f);
			}
		}
		if (bossVulnerable) {
			best = max(best, bossDwell / 0.72f);
		}
	}
	return ofClamp(best, 0.0f, 1.0f);
}

float ofApp::hitRadiusFor(const Meteor& meteor) const {
	const float depth01 = ofMap(meteor.position.z, farZ, nearZ, 0.0f, 1.0f, true);
	return ofLerp(meteor.heavy ? 58.0f : 42.0f, meteor.heavy ? 150.0f : 120.0f, depth01);
}

bool ofApp::isMouseOver(const Meteor& meteor) const {
	if (meteor.exploding) {
		return false;
	}

	const ofVec2f screen = screenPositionFor(meteor);
	const float distance = screen.distance(aimPosition);
	return distance <= hitRadiusFor(meteor);
}

ofVec2f ofApp::screenPositionFor(const Meteor& meteor) const {
	const glm::vec3 screen = camera.worldToScreen(glm::vec3(meteor.position.x, meteor.position.y, meteor.position.z));
	return ofVec2f(screen.x, screen.y);
}

ofVec3f ofApp::bossWorldPosition() const {
	const float t = ofGetElapsedTimef();
	return ofVec3f(
		sinf(t * 0.85f) * 185.0f + sinf(t * 1.7f) * 42.0f,
		-45.0f + sinf(t * 1.05f) * 38.0f,
		-820.0f + cosf(t * 0.62f) * 45.0f);
}

ofVec2f ofApp::bossScreenPosition() const {
	const ofVec3f bossPosition = bossWorldPosition();
	const glm::vec3 screen = camera.worldToScreen(glm::vec3(bossPosition.x, bossPosition.y, bossPosition.z));
	return ofVec2f(screen.x, screen.y);
}

ofVec2f ofApp::shipScreenPosition() const {
	const glm::vec3 screen = camera.worldToScreen(glm::vec3(0, -350, 520));
	return ofVec2f(screen.x, screen.y);
}

void ofApp::launchMissileAt(const Meteor& meteor) {
	Missile missile;
	missile.start = shipScreenPosition();
	missile.end = screenPositionFor(meteor);
	missile.color = meteor.color;
	missiles.push_back(missile);
	triggerSound(0, 0.65f, ofMap(missile.end.x, 0, ofGetWidth(), 0.12f, 0.88f, true));
}

void ofApp::launchMissileAt(const ofVec2f& target, const ofColor& color) {
	Missile missile;
	missile.start = shipScreenPosition();
	missile.end = target;
	missile.color = color;
	missiles.push_back(missile);
	triggerSound(0, 0.65f, ofMap(missile.end.x, 0, ofGetWidth(), 0.12f, 0.88f, true));
}

void ofApp::triggerSound(int type, float intensity, float pan) {
	std::lock_guard<std::mutex> lock(audioMutex);
	if (synthVoices.size() > 28) {
		synthVoices.erase(synthVoices.begin());
	}

	SynthVoice voice;
	voice.type = type;
	voice.volume = 0.16f * intensity;
	voice.pan = ofClamp(pan, 0.05f, 0.95f);
	voice.noiseSeed = (unsigned int)ofRandom(1, 0x00FFFFFF);

	if (type == 0) {
		voice.duration = 0.16f;
		voice.freqStart = ofRandom(760.0f, 980.0f);
		voice.freqEnd = ofRandom(280.0f, 420.0f);
		voice.volume = 0.12f * intensity;
	} else if (type == 1) {
		voice.duration = 0.24f;
		voice.freqStart = ofRandom(210.0f, 310.0f);
		voice.freqEnd = ofRandom(70.0f, 120.0f);
		voice.volume = 0.18f * intensity;
	} else if (type == 2) {
		voice.duration = 0.48f;
		voice.freqStart = ofRandom(90.0f, 150.0f);
		voice.freqEnd = ofRandom(30.0f, 55.0f);
		voice.volume = 0.28f * intensity;
	} else if (type == 4) {
		voice.duration = 0.28f;
		voice.freqStart = ofRandom(820.0f, 980.0f);
		voice.freqEnd = ofRandom(520.0f, 620.0f);
		voice.volume = 0.24f * intensity;
	} else {
		voice.duration = 0.62f;
		voice.freqStart = ofRandom(360.0f, 460.0f);
		voice.freqEnd = ofRandom(95.0f, 135.0f);
		voice.volume = 0.18f * intensity;
	}

	synthVoices.push_back(voice);
}

void ofApp::spawnExplosionParticles(const Meteor& meteor, int amount) {
	spawnExplosionParticles(meteor.position, meteor.color, amount);
}

void ofApp::spawnExplosionParticles(const ofVec3f& position, const ofColor& color, int amount) {
	for (int i = 0; i < amount; ++i) {
		Particle particle;
		particle.position = position;
		particle.velocity.set(ofRandom(-1.0f, 1.0f), ofRandom(-1.0f, 1.0f), ofRandom(-0.7f, 1.1f));
		particle.velocity.normalize();
		particle.velocity *= ofRandom(160.0f, 560.0f);
		particle.color = color.getLerped(ofColor(255, 235, 120), ofRandom(0.15f, 0.75f));
		particle.lifetime = ofRandom(0.42f, 0.95f);
		particle.size = ofRandom(3.0f, 10.0f);
		particles.push_back(particle);
	}
}

void ofApp::activatePowerShot() {
	if (powerCharge < 100.0f) {
		return;
	}

	powerCharge = 0.0f;
	powerPulse = 1.0f;
	screenShake = 0.45f;
	triggerSound(3, 1.15f, ofMap(aimPosition.x, 0, ofGetWidth(), 0.1f, 0.9f, true));

	for (auto& meteor : meteors) {
		if (meteor.exploding) {
			continue;
		}

		const float distance = screenPositionFor(meteor).distance(aimPosition);
		if (distance < 330.0f) {
			meteor.dwell = dwellToExplode;
			explodeMeteor(meteor);
		}
	}

	for (auto& orb : bossOrbs) {
		if (orb.exploding) {
			continue;
		}

		const glm::vec3 screen3 = camera.worldToScreen(glm::vec3(orb.position.x, orb.position.y, orb.position.z));
		const ofVec2f screen(screen3.x, screen3.y);
		if (screen.distance(aimPosition) < 360.0f) {
			launchMissileAt(screen, orb.color);
			spawnExplosionParticles(orb.position, orb.color, 18);
			orb.exploding = true;
			orb.explosion = 0.0f;
			if (bossActive && !bossVulnerable) {
				bossOrbsDestroyedThisPhase++;
			}
			score += 60 + combo * 5;
		}
	}

	if (bossActive && bossVulnerable && bossScreenPosition().distance(aimPosition) < 360.0f) {
		damageBoss();
	}
}

void ofApp::explodeMeteor(Meteor& meteor) {
	if (meteor.exploding) {
		return;
	}

	launchMissileAt(meteor);
	if (heavyShotTimer > 0.0f) {
		launchMissileAt(screenPositionFor(meteor) + ofVec2f(20.0f, -14.0f), ofColor(255, 155, 45));
	}

	const int shotDamage = heavyShotTimer > 0.0f ? 2 : 1;
	if (meteor.hitPoints > shotDamage) {
		meteor.hitPoints -= shotDamage;
		meteor.dwell = 0.0f;
		meteor.speed = std::max(meteor.speed * 0.88f, 80.0f);
		spawnExplosionParticles(meteor, heavyShotTimer > 0.0f ? 13 : 8);
		triggerSound(1, meteor.heavy ? 1.05f : 0.7f, ofMap(screenPositionFor(meteor).x, 0, ofGetWidth(), 0.1f, 0.9f, true));
		score += ((meteor.heavy ? 60 : 25) + combo * 5) * shotDamage;
		powerCharge = ofClamp(powerCharge + 5.0f * shotDamage, 0.0f, 100.0f);
		screenShake = std::max(screenShake, 0.28f);
		return;
	}

	spawnExplosionParticles(meteor, heavyShotTimer > 0.0f ? 32 : 22);
	triggerSound(2, meteor.heavy ? 1.25f : 0.92f, ofMap(screenPositionFor(meteor).x, 0, ofGetWidth(), 0.1f, 0.9f, true));
	meteor.exploding = true;
	meteor.explosion = 0.0f;
	score += (meteor.heavy ? 450 : 100) + combo * 25;
	combo++;
	powerCharge = ofClamp(powerCharge + 14.0f + combo * 0.8f, 0.0f, 100.0f);
	destroyedInWave++;
	if (destroyedInWave >= meteorsPerWave) {
		startNextWave();
	}
}

void ofApp::startNextWave() {
	wave++;
	destroyedInWave = 0;
	meteorsPerWave = std::min(4 + (wave - 1), 7);
	speedMultiplier = std::min(1.0f + wave * 0.045f, 1.38f);
	waveMessageTimer = 1.8f;
	triggerSound(3, 0.8f, 0.5f);
}
