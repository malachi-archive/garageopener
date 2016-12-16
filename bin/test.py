#!/usr/bin/python 

#from click import click
import click
import os

os.environ['x'] = '100000'

if click.confirm("test", default='Y'):
    click.echo("Did it")
