from distutils.core import setup

setup(
    name='argonaut',
    version='0.1.0',
    author='Mike Burns and Matt Horan',
    author_email='mike@mike-burns.com',
    scripts=['bin/argonaut'],
    url='http://github.com/argonaut/argonaut',
    license='LICENSE',
    description='A spatial file manager',
    long_description=open('README.md').read(),
    requires=[
        "pygobject (>= 2.12.3)",
    ],
)
