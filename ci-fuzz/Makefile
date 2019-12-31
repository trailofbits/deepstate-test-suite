
# build all locally and with virtual environment
devbuild:
	virtualenv --python=python3 venv
	source venv/bin/activate

	python fuzzbed-cli/setup.py install
	python orchestrator-setup.py install

# build
build: bootstrap

bootstrap:
	docker build -t fuzzbed_orchestrator ./orchestrator
